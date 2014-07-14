<?php
/**
 * Render a list of customers in the {users} database.
 */
function local_app_customer_list() {
  global $user;
  // Check if user has permission to access this page.
  if(!user_access('view admin pages')) {
    return MENU_ACCESS_DENIED;
  }

  // Grab fields from users table.
  $entries = db_select('users', 'u')
    ->fields('u', array('uid','first_name','last_name','name','account_bal'))
    ->condition('uid', 1, '>')
      // Return the result in object format.
      ->execute();
  $output = '';

  // Get all entries in the {users} table.
  if (isset($entries)) {
    // Create header array for table
    $header = array(
      'uid' => t('UID'),
      'first_name' => t('First Name'),
      'last_name' => t('Last Name'),
      'user_name' => t('Username'),
      'account_bal' => t('Account Balance'),
      'edit_user' => t('Edit User'),
      'delete_user' => t('Delete User')
    );

    ctools_include('ajax');
    ctools_include('modal');
    ctools_modal_add_js();
    //ctools_add_js('ajax-responder');

    // Create our own javascript that will be used to theme a modal.
    $sample_style = array(
      'local-app-edit-customer-modal-style' => array( // local_app_modal_style is the identifier to our modal theme
        'modalSize' => array(
          'type' => 'fixed',
          'width' => 460,
          'height' => 680,
          'addWidth' => 10,
          'addHeight' => 10,
          'contentRight' => 0,
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
            'alt' => t('Close'), 'title' => t('Close'))
        ),
        'closeText' => "<span id='close-modal-button'>Close</span>",
        /*'throbber' => theme('image', array(
          'path' => ctools_image_path('ajax-loader.gif', 'local_app'),
          'alt' => t('Loading...'), 'title' => t('Loading'))),*/
      ),
      // add another style for delete confirm page popup
      'local-app-delete-customer-modal-style' => array( // local_app_modal_style is the identifier to our modal theme
        'modalSize' => array(
          'type' => 'fixed',
          'width' => 600,
          'height' => 150,
          'addWidth' => 0,
          'addHeight' => 10,
          'contentRight' => 0,
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
            'alt' => t('Close'), 'title' => t('Close'))
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
    $rows = array();
    // Build the $rows array
    foreach ($entries as $key => $entry) {
      // Sanitize the data before handing it off to the theme layer.
      $entry_arr = array_map('check_plain', (array) $entry);

      // Link for modal forms
      $links['edit_user_form'] = ctools_modal_text_button(t('Edit User'), 'local_app/admin/customer_list/nojs/edit/' . $entry->uid,
        t('Edit User Data'), 'ctools-modal-local-app-edit-customer-modal-style');

      $links['delete_user_form'] = ctools_modal_text_button(t('Delete User'), 'local_app/admin/customer_list/nojs/delete/' . $entry->uid,
        t('Edit User Data'), 'ctools-modal-local-app-delete-customer-modal-style');

      // Build the rows and add data
      // Add span tags with UID as identifier for ajax command in submit function
      // to remove row after deleting user with modal form.
      $rows[] = array(
        'uid' => '<span id="local-app-modal-ajax-' . $entry->uid . '">' . $entry_arr['uid'],
        'first_name' => $entry_arr['first_name'],
        'last_name'=> $entry_arr['last_name'],
        'user_name' => $entry_arr['name'],
        'account_bal' => '$' . $entry_arr['account_bal'],
        'edit_user' => $links['edit_user_form'],
        'delete_user' => $links['delete_user_form'] . '</span>',
      );

    }

    $output = theme('table', array('header' => $header, 'rows' => $rows));
    //dsm($output);
  } else {
    drupal_set_message(t('No entries have been added yet.'));
  }

  return $output;
}
