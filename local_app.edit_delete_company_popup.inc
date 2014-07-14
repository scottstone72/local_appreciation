<?php

/**
 * Ajax menu callback is a popup modal form for deleting users.
 * Calls function local_app_delete_user_form to show the form in the modal.
 */
function local_app_delete_company($js = NULL) {
  if ($js) {

    ctools_include('ajax');
    ctools_include('modal');

    $form_state = array(
      'ajax' => TRUE,
      'title' => t('Delete Company'),
    );
    // Use ctools to generate ajax instructions for the browser to create
    // a form in a modal popup.
    $output = ctools_modal_form_wrapper('local_app_delete_company_form', $form_state);

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
    return drupal_get_form('local_app_delete_company_form');
  }
}

/**
 * This is the callback the ajax link calls for the delete user modal popup form or form page.
 */
function local_app_delete_company_form($form,&$form_state) {
  global $user;
  // Check if user has permission to access this page.
  if(!user_access('view admin pages')) {
    return MENU_ACCESS_DENIED;
  }

  // Get the uid number from path that was added in the edit/delete link in the
  // local_app_customer_list_form table.

  $path = current_path();
  $company_id = preg_replace("/[^0-9]/", '', $path);

  // Grab fields from users table.
  $entries = db_select('local_app_company', 'l')
    ->fields('l', array('id','company_name','image_file_id','image_path'))
    ->condition('id', $company_id)
    // Return the result in object format.
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
    );

    $form['user_data'] = array(
      '#type' => 'hidden',
      '#value' => $data,
    );
    $form['company_image'] = array(
      '#type' => 'markup',
      '#markup' => $image,
    );
    $form['company_info'] = array(
      '#type' => 'markup',
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#markup' => t('Are you sure you want to delete <span id="delete-name"><strong>@company_name</strong><span> from the database?<br/>', array(
        '@company_name' => $entry->company_name)),
    );
  }
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
//      '#id' => 'my-submit-button',
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

  return $form;
}

/**
 * Submit function for deleting user
 */
function local_app_delete_company_form_submit($form,&$form_state) {

  $id = $form_state['values']['user_data']['id'];
  $fid = $form_state['values']['user_data']['image_file_id'];
  $company_name = $form_state['values']['user_data']['company_name'];

  $file = file_load($fid);
  // Delete the company image
  file_delete($file);
  // Then delete the company
  db_delete('local_app_company')
    ->condition('id', $id)
    ->execute();

  // Remove row from table after user chooses to delete in our modal form.
  $form_state['ajax_commands'][] = ajax_command_remove('tr:has(#local-app-modal-ajax-' . $id . ')');
  // Remove message if old message is there after an edit.
  $form_state['ajax_commands'][] = ajax_command_remove('.user-message');
  // Show the user a message that company was deleted.
  $form_state['ajax_commands'][] = ajax_command_after('ul.tabs.primary',
    "<div class='user-message'><strong>$company_name</strong> has successfully been deleted from the database.</div>");

  // Restripe table after we remove and replace table row.
  $form_state['ajax_commands'][] = ajax_command_restripe('table');
  // Tell the browser to close the modal.
  $form_state['ajax_commands'][] = ctools_modal_command_dismiss();
}


/**
 * Ajax menu callback for edit user popup modal for customer page.
 */
function local_app_edit_company($js = NULL) {
  if ($js) {

    ctools_include('ajax');
    ctools_include('modal');

    $form_state = array(
      'ajax' => TRUE,
      'title' => t('Edit Coupon Value'),
    );

    // Use ctools to generate ajax instructions for the browser to create
    // a form in a modal popup.
    $output = ctools_modal_form_wrapper('local_app_edit_company_form', $form_state);

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
    return drupal_get_form('local_app_edit_company_form');
  }
}

/**
 * This is the callback the ajax link calls for the edit user modal popup form or form page.
 */
function local_app_edit_company_form($form,&$form_state) {
  global $user;
  // Check if user has permission to access this page.
  if(!user_access('view admin pages')) {
    return MENU_ACCESS_DENIED;
  }

  // Get the company_id number from path that was added in the edit/delete link in the
  // local_app_company_list table.
  $path = current_path();
  $company_id = preg_replace("/[^0-9]/", '', $path);

 $query = db_select('local_app_company','l')
    ->fields('l', array('company_name','coupon_value','image_path'))
    ->condition('id', $company_id)
    ->execute();



  foreach($query as $value) {
    // Display and style image in the company list.
    $img_url = $value->image_path;  // the orig image uri
    $style = 'thumbnail';  // or any other custom image style you've created via /admin/config/media/image-styles
    $image = '<img src="' . image_style_url($style, $img_url) .  '" id="company-modal-image">';

    $company_name = $value->company_name;

    $form['company_id'] =array(
      '#type' => 'hidden',
      '#value' => $company_id,
    );
    $form['company_name'] =array(
      '#type' => 'hidden',
      '#value' => $company_name,
    );
    $form['company_image'] = array(
      '#type' => 'markup',
      '#markup' => $image,
    );
    $form['company_heading'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' . 'Coupon value for <strong>' . $company_name . '</strong></p>',
    );

    $form['coupon_value'] = array(
      '#type'  => 'textfield',
      '#title' => t('Coupon Value'),
      '#size'  => 20,
      '#field_prefix' => '$',
      '#description' => t('Enter coupon amount.'),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
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
  }
  return $form;
}

/**
 * Validate function for edit user modal popup.
 */
function local_app_edit_company_form_validate($form, &$form_state) {
  // Sanitize the user submitted data before we save to database.
$form_state['values']['coupon_value'] = check_plain($form_state['values']['coupon_value']);

}

/**
 * Submit function for edit user modal popup.
 */
function local_app_edit_company_form_submit($form, &$form_state) {

  $cid = $form_state['values']['company_id'];
  $company_name = $form_state['values']['company_name'];
  $coupon_value = $form_state['values']['coupon_value'];
  // Lets save this companies new coupon value.
  $result = db_update('local_app_company')
    ->fields(array('coupon_value' => $coupon_value))
    ->condition('id',$cid)
    ->execute();

  $form_state['ajax_commands'][] = ajax_command_remove('.user-message');
//
  $form_state['ajax_commands'][] = ajax_command_after('ul.tabs.primary',
    "<div class='user-message'>You have updated the <strong>$company_name</strong> coupon to be worth <strong>$$coupon_value</strong></div>");

//  // prepare html for table so we can replace row
  $html = '<span id="coupon-value-' . $cid . '">' . $coupon_value . '</span>';
  // Set selector to element we want to replace.
  $selector = "span#coupon-value-$cid";
  // Replace row in table after updating a user in our modal form.
  $form_state['ajax_commands'][] = ajax_command_replace($selector,$html);
  // Tell the browser to close the modal.
  $form_state['ajax_commands'][] = ctools_modal_command_dismiss();
}