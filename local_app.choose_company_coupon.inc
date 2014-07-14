<?php
/**
 * This is a page for customers to make a choice of what coupon
 * they would like receive.
 */
function local_app_choose_company_coupon() {
  global $user;
  // Check if user has permission to access this page.
  if(!user_access('view company rewards page')) {
    return MENU_ACCESS_DENIED;
  }

  // Get companies from database table
  $result = db_select('local_app_company','l')
    ->fields('l', array('id','company_name','image_path','image_file_id','coupon_value'))
    ->execute();

  if(isset($result)) {

      $output = array();
      $rows = array();

    ctools_include('ajax');
    ctools_include('modal');
    ctools_modal_add_js();

    // Create our own javascript that will be used to theme a modal.
    $sample_style = array(
      'local-app-choose-coupon-modal-style' => array( // local_app_modal_style is the identifier to our modal theme
        'modalSize' => array(
          'type' => 'fixed',
          'width' => 600,
          'height' => 280,
          'addWidth' => 0,
          'addHeight' => 10,
          'contentRight' => 40,
          'contentBottom' => 0,
        ),
        'modalOptions' => array(
          'opacity' => .6,
          'background-color' => 'black',
        ),
        'animation' => 'fadeIn',
        'animationSpeed' => 'fast',
        'modalTheme' => 'local_app',// append our custom theme to end of namespace in js file
        'closeImage' => theme('image', array(
            'path' => ctools_image_path('icon-close-window.png', 'local_app', 'images'),
            'alt' => t('Close window'), 'title' => t('Close window'))
        ),
        'closeText' => "<span id='close-modal-button'>Close</span>",
        /*'throbber' => theme('image', array(
          'path' => ctools_image_path('ajax-loader.gif', 'local_app'),
          'alt' => t('Loading...'), 'title' => t('Loading'))),*/
      ),
    );

    drupal_add_js($sample_style, 'setting');// add our js settings for the modal popup
    // load our custom css and js files for our custom modal
    ctools_add_js('local_app','local_app','js'); // load our custom js file
    ctools_add_css('local_app','local_app','css');// load our custom css file

      // Build the $rows array
      foreach ($result as $entry) {
        // Sanitize the data before handing it off to the theme layer.
        $entry_arr = array_map('check_plain', (array) $entry);
        $text = $entry_arr['company_name'];

        // Link for modal forms
        $links['coupon_choice'] = ctools_modal_text_button(t($text), 'local_app/choose_company_coupon/nojs/' . $entry_arr['id'],
          t('Edit Coupon Value'), 'ctools-modal-local-app-choose-coupon-modal-style');

        // Display and style image in the company list.
        $img_url = $entry_arr['image_path'];  // the orig image uri
        $style = 'thumbnail';  // or any other custom image style you've created via /admin/config/media/image-styles

        $rows[] = array(
          //'id' => '<span id="local-app-modal-ajax-' . $entry_arr['id'] . '">' . $entry_arr['id'],
          'image_path' => '<span id="local-app-modal-ajax-' . $entry_arr['id'] . '"><img src="' . image_style_url($style, $img_url) .  '">',
          'company_name' => $links['coupon_choice'],
          'coupon_value' => '<span id="coupon-value-' . $entry_arr['id']  . '">$' . $entry_arr["coupon_value"] . ' coupon value</span>',
        );
      }

    $output = t("<div id='local-app-customer-message'><h2 id='customer-info'>Hello <strong>@first_name @last_name</strong>. You currently have <span id='account-bal'><strong>$@account_bal</strong></span>" .
      " in your account available.</h2>" . "<strong>Crystal Metz</strong> would like to thank you for participating in the <strong><em>Local Appreciation Rewards Program</em></strong> and has" .
      " rewarded you with points that can be redeemed for rewards from local participating companies." . " <strong>Click a company name to redeem your rewards points. </strong></div><br/>", array(
        '@account_bal' => $user->account_bal, '@first_name' => $user->first_name, '@last_name' => $user->last_name));
    $output .= theme('table', array('rows' => $rows));
    return $output;
  }

}

/**
 * Ajax menu callback is a popup modal form for choosing coupon.
 * Calls function local_app_choose_coupon_form to show the form in the modal.
 */
function local_app_choose_coupon($js = NULL) {
  if ($js) {
    ctools_include('ajax');
    ctools_include('modal');

    $form_state = array(
      'ajax' => TRUE,
      'title' => t('Choose Company?'),
    );
    // Use ctools to generate ajax instructions for the browser to create
    // a form in a modal popup.
    $output = ctools_modal_form_wrapper('local_app_choose_coupon_form', $form_state);

    // If the form has been submitted, there may be additional instructions
    // such as dismissing the modal popup.
    if (!empty($form_state['ajax_commands'])) {
      $output = $form_state['ajax_commands'];
    }
    // Return the ajax instructions to the browser via ajax_render().
    print ajax_render($output);
    drupal_exit($destination = NULL);
  }
  else {
    // Return just the page with the form if no js is enabled.
    return drupal_get_form('local_app_choose_coupon_form');
  }
}
/**
 * This is the callback the ajax link calls for the Choose Coupon modal popup form.
 */
function local_app_choose_coupon_form($form,&$form_state) {
  global $user;
  // Check if user has permission to access this page.
  if(!user_access('view company rewards page')) {
    return MENU_ACCESS_DENIED;
  }

  ctools_add_css('local_app','local_app','css');// load our custom css file

  // Get the company id number from path that was added in the link in the
  // local_app_choose_company_coupon table.
  $path = current_path();
  $company_id = preg_replace("/[^0-9]/", '', $path);
  $data = array();


  // Grab fields from users table.
  $entries = db_select('local_app_company', 'l')
    ->fields('l', array('id','company_name','image_file_id','coupon_value','image_path'))
    ->condition('id', $company_id)
    ->execute();

  foreach($entries as $entry) {

    // Display and style image in the company list.
    $img_url = $entry->image_path;  // the orig image uri
    $style = 'thumbnail';  // or any other custom image style you've created via /admin/config/media/image-styles
    $image = '<img src="' . image_style_url($style, $img_url) .  '" id="company-modal-image">';

    $data = array(
      'id' => $entry->id,
      'company_name' => $entry->company_name,
      'image_file_id' => $entry->image_file_id,
      'coupon_value' => $entry->coupon_value,
      //'image_path' => $image,
    );

    $form['company_data'] = array(
      '#type' => 'hidden',
      '#value' => $data,
    );
    // Hide form elements and give error in modal if user account_bal is less than coupon value.
    // Otherwise show the form elements.
    // Check to see if user has enough money in their account
    if($user->account_bal >= $data['coupon_value']) {
      $form['company_image'] = array(
        '#type' => 'markup',
        '#markup' => $image,
      );
      $form['company_info'] = array(
        '#type' => 'markup',
        '#prefix' => '<p>',
        '#suffix' => '</p>',
        '#markup' => t('You are choosing a <strong>$@coupon_value</strong> value coupon for <span id="choose-name"><strong>@company_name</strong><span>. Click the button below to confirm your selection.<br/>', array(
          '@company_name' => $entry->company_name, '@coupon_value' => $entry->coupon_value)),
      );
      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Send Me Coupon'),
//        '#id' => 'my-submit-button',
      );
      $form['cancel'] = array(
        '#markup' => l(t('Cancel'), '#', array(
            'attributes' => array(
              'class' => 'ctools-close-modal'
            ),
            'external' => TRUE
          )
        )
      );
    } else {
      form_set_error('div#breadcrumb',t('Sorry! You do not have enough funds in your account for this company coupon. Please make another choice.'));
    }
  }

  return $form;
}

/**
 * Validate that the user has enough money in their account
 */
function local_app_choose_coupon_form_validate($form,&$form_state) {

  global $user;
  $coupon_value =  $form_state['values']['company_data']['coupon_value'];
  // Do the calculations needed to users account balance.
  if($user->account_bal >= $coupon_value) {
    // Do calculation
    $form_state['values']['company_data']['new_account_bal'] = ($user->account_bal) - $coupon_value;
  }
}

/**
 *  Submit coupon choice made by user
 */
function local_app_choose_coupon_form_submit($form,&$form_state) {

  global $user;
  $coupon_value =  $form_state['values']['company_data']['coupon_value'];
  $company_name = $form_state['values']['company_data']['company_name'];
  $user->account_bal = $form_state['values']['company_data']['new_account_bal'];
  if(isset($form_state['values']['company_data']['new_account_bal'])) {
    // Update the users account.
    user_save($user, $user->account_bal);
  }

  $form_state['ajax_commands'][] = ajax_command_remove('div#user-message');

  $form_state['ajax_commands'][] = ajax_command_replace('#account-bal', "<span id='account-bal'><strong>$$user->account_bal</strong></span>");

  $form_state['ajax_commands'][] = ajax_command_before('h1#page-title',
    "<div class='user-message'>Thank you! <strong>Crystal Metz Local Appreciation</strong> values our customers.<br/> You will be sent a <strong>$company_name</strong> coupon worth <strong>$$coupon_value</strong></div>");

//  // Tell the browser to close the modal.
  $form_state['ajax_commands'][] = ctools_modal_command_dismiss();
}