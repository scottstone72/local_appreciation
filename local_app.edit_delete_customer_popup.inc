<?php

/**
 * Ajax menu callback for edit user popup modal for customer page.
 */
function local_app_edit_user($ajax) {
  if ($ajax) {

    ctools_include('ajax');
    ctools_include('modal');

    $form_state = array(
      'ajax' => TRUE,
      'title' => t('Update user information'),
    );

    // Use ctools to generate ajax instructions for the browser to create
    // a form in a modal popup.
    $output = ctools_modal_form_wrapper('local_app_edit_user_form', $form_state);

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
    return drupal_get_form('local_app_edit_user_form');
  }
}

/**
 * This is the callback the ajax link calls for the edit user modal popup form or form page.
 */
function local_app_edit_user_form($form,&$form_state) {
  global $user;
  // Check if user has permission to access this page.
  if(!user_access('view admin pages')) {
    return MENU_ACCESS_DENIED;
  }
  // Get the uid number from path that was added in the edit/delete link in the
  // local_app_customer_list_form table.
  $path = current_path();
  $user_uid = preg_replace("/[^0-9]/", '', $path);
  // load the user object from database
  $account = user_load($user_uid);

    $form['user_uid'] = array(
      '#type' => 'hidden',
      '#value' => $user_uid,
    );

    $form['first_name'] = array(
      '#title' => 'First Name',
      '#type' => 'textfield',
      '#value' => $account->first_name,
    );
    $form['last_name'] = array(
      '#title' => 'Last Name',
      '#type' => 'textfield',
      '#value' => $account->last_name,
    );
    $form['user_name'] = array(
      '#title' => 'Username',
      '#description' => 'Edit username.',
      '#type' => 'textfield',
      '#value' => $account->name,
    );
    $form['local_app_pass'] = array(
      '#title' => 'Password',
      '#description' => 'Enter a password.',
      '#type' => 'textfield',
      '#value' => '',
    );
    $form['mail'] = array(
      '#title' => 'Email',
      '#description' => 'Enter a valid email address.',
      '#type' => 'textfield',
      '#value' => $account->mail,
    );
    $form['account_bal'] = array(
      '#title' => 'Account Balance',
      '#description' => 'Enter dollar($) amount.',
      '#default_value' => number_format(0.00,2),
      '#type' => 'textfield',
      '#value' => $account->account_bal,
    );
    $form['customer_notes'] = array(
      '#title' => 'Customer Info',
      '#type' => 'textarea',
      '#rows' => 4,
      '#value' => $account->customer_notes,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
    );
    $form['actions']['cancel'] = array(
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
 * Validate function for edit user modal popup.
 */
function local_app_edit_user_form_validate($form, &$form_state) {
  // Sanitize the user submitted data before we save to database.
  $form_state['values']['user_uid'] = check_plain($form_state['values']['user_uid']);
  $form_state['values']['first_name'] = check_plain($form_state['input']['first_name']);
  $form_state['values']['last_name'] = check_plain($form_state['input']['last_name']);
  $form_state['values']['user_name'] = check_plain($form_state['input']['user_name']);
  $form_state['values']['local_app_pass'] = check_plain($form_state['input']['local_app_pass']);
  $form_state['values']['mail'] = check_plain($form_state['input']['mail']);
  $form_state['values']['account_bal'] = check_plain($form_state['input']['account_bal']);
  $form_state['values']['customer_notes'] = check_plain($form_state['input']['customer_notes']);

}

/**
 * Submit function for edit user modal popup.
 */
function local_app_edit_user_form_submit($form, &$form_state) {

  $user_uid = $form_state['values']['user_uid'];
  // load user again here in the submit function
  $account = user_load($user_uid);

  $edit = array(
    'name' => $form_state['values']['user_name'],
    'pass' => user_password(),
    'mail' => $form_state['values']['mail'],
    'init' => $form_state['values']['mail'],
    'status' => 1,
    'access' => REQUEST_TIME,
    // Adding extra fields
    'first_name' => $form_state['values']['first_name'],
    'last_name' => $form_state['values']['last_name'],
    'account_bal' => $form_state['values']['account_bal'],
    'customer_notes' => $form_state['values']['customer_notes'],
  );
  // Update the users account.
  user_save($account, $edit);

  // prepare html for table so we can replace row
  $html = '<tr class=""><td><span id="local-app-modal-ajax-' . $user_uid . '">' . $user_uid . '</td>';
  $html .= '<td>' . $edit['first_name'] . '</td><td>' . $edit['last_name']. '</td><td>' . $edit['name'] . '</td>';
  $html .= '<td>' . '$' . $edit['account_bal'] . '</td>';
  $html .= '<td><a href="/local_app/admin/customer_list/nojs/edit/' . $user_uid  . '" ' . 'class="ctools-use-modal ctools-modal-local-app-edit-modal-style"';
  $html .= 'title="Edit User Data">Edit User</a></td><td><a href="/local_app/admin/customer_list/nojs/delete/' . $user_uid . '" ';
  $html .= 'class="ctools-use-modal ctools-modal-local-app-delete-modal-style"' .
            'title="Edit User Data">Delete User</a></span></td> </tr>';

  // Resend a new username and password if changing customer info
  $date = date('Y-m-d h:i:s');
  // Send email to administrator notifying him of request.
  $to = $form_state['values']['mail'];
  $from = $user_uid->mail;

  $params['date'] = $date;
  $params['first_name'] = $form_state['values']['first_name'];
  $params['last_name'] =  $form_state['values']['last_name'];
  $params['user_name'] =  $form_state['values']['user_name'];
  $params['local_app_pass'] = $form_state['values']['local_app_pass'];
  $params['account_bal'] = $form_state['values']['account_bal'];

  $sent = drupal_mail('local_app', 'edit_user', $to, language_default(), $params, $from, TRUE);
  // Show error if mail does not get sent for whatever reason.
  if(!$sent) {
    drupal_set_message(t('An error occurred and admin mail was not sent. Please notify administrator.'), 'error');
  }
  // Remove message if old message is there after an edit.
  $form_state['ajax_commands'][] = ajax_command_remove('.user-message');
  // Show a message to user confirming customer edits.
  $form_state['ajax_commands'][] = ajax_command_after('ul.tabs.primary',
    "<div class='user-message'><strong>Changes made to the users account have been saved.</strong></div>");

  // Set selector to element we want to replace.
  $selector = 'tr:has(#local-app-modal-ajax-' . $user_uid . ')';
  // Replace row in table after updating a user in our modal form.
  $form_state['ajax_commands'][] = ajax_command_replace($selector,$html);
  // Restripe table after we remove and replace table row.
  $form_state['ajax_commands'][] = ajax_command_restripe('table');
  // Tell the browser to close the modal.
  $form_state['ajax_commands'][] = ctools_modal_command_dismiss();
}





/**
 * Ajax menu callback is a popup modal form for deleting users.
 * Calls function local_app_delete_user_form to show the form in the modal.
 */
function local_app_delete_user($js = NULL) {
  if ($js) {

    ctools_include('ajax');
    ctools_include('modal');

    $form_state = array(
      'ajax' => TRUE,
      'title' => t('Delete User'),
    );
    // Use ctools to generate ajax instructions for the browser to create
    // a form in a modal popup.
    $output = ctools_modal_form_wrapper('local_app_delete_user_form', $form_state);

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
    return drupal_get_form('local_app_delete_user_form');
  }
}

/**
 * This is the callback the ajax link calls for the delete user modal popup form or form page.
 */
function local_app_delete_user_form($form,&$form_state) {
  global $user;
  // Check if user has permission to access this page.
  if(!user_access('view admin pages')) {
    return MENU_ACCESS_DENIED;
  }

  // Get the uid number from path that was added in the edit/delete link in the
  // local_app_customer_list_form table.
  global $user_uid;
  $path = current_path();
  $user_uid = preg_replace("/[^0-9]/", '', $path);

  // Grab fields from users table.
  $entries = db_select('users', 'u')
    ->fields('u', array('uid','first_name','last_name'))
    ->condition('uid', $user_uid)
    // Return the result in object format.
    ->execute();

  foreach($entries as $entry) {

    $data = array(
      'uid' => $user_uid,
      'first_name' => $entry->first_name,
      'last_name' => $entry->last_name,
    );

    $form['user_data'] = array(
      '#type' => 'hidden',
      '#value' => $data,
    );

    $form['customer_info'] = array(
      '#type' => 'markup',
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#markup' => t('Are you sure you want to delete <span id="delete-name"><strong>@first_name @last_name</strong><span> from the database?<br/>', array(
          '@first_name' => $entry->first_name, '@last_name' => $entry->last_name)),
    );
  }
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#id' => 'my-submit-button',
    );
    $form['actions']['cancel'] = array(
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
function local_app_delete_user_form_submit($form,&$form_state) {

  user_delete($form_state['values']['user_data']['uid']);

  // Remove row from table after user chooses to delete in our modal form.
  $form_state['ajax_commands'][] = ajax_command_remove('tr:has(#local-app-modal-ajax-' . $form_state['values']['user_data']['uid'] . ')');

  // Show a message to user confirming customer edits.
  $form_state['ajax_commands'][] = ajax_command_after('ul.tabs.primary',
    "<div class='user-message'><strong>You have successfully deleted the users account.</strong></div>");

  // Restripe table after we remove and replace table row.
  $form_state['ajax_commands'][] = ajax_command_restripe('table');
  // Tell the browser to close the modal.
  $form_state['ajax_commands'][] = ctools_modal_command_dismiss();
}
