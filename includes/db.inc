<?php

/**
 * @file
 * Database functionality for tracking job status in Islandora Job.
 */

/**
 * Inserts a job to have its state tracked.
 *
 * @param string $data
 *   A JSON encoded string representing the payload data.
 *
 * @return int
 *   The ID for the job's status in the database.
 */
function islandora_job_insert_job_status($data) {
  return db_insert('islandora_job_job_status')
    ->fields(array(
      'data' => $data,
    ))
    ->execute();
}

/**
 * Updates a job's state.
 *
 * @param string $status_id
 *   The ID of the job being tracked to be updated in this status table.
 * @param int $status
 *   The state of the job being created or updated.
 */
function islandora_job_update_job_status($status_id, $status) {
  db_update('islandora_job_job_status')
    ->fields(array(
      'state' => $status,
    ))
    ->condition('status_id', $status_id)
    ->execute();
}

/**
 * Updates a job's state given a payload.
 *
 * @param string $data
 *   The data of the payload being used to identify the job.
 *
 * @return int
 *   The ID of the status to be used for removal later.
 */
function islandora_job_start_job_status($data) {
  // XXX: Note this is not a race condition as there will only ever be one job
  // in the waiting state due to the deduplication introduced.
  // Get the status ID of the database to use to remove itself later on.
  $status_id = db_select('islandora_job_job_status', 'i')
    ->fields('i', array('status_id'))
    ->condition('data', $data)
    ->condition('state', ISLANDORA_JOB_STATE__WAITING)
    ->execute()
    ->fetchField();

  db_update('islandora_job_job_status')
    ->fields(array(
      'state' => ISLANDORA_JOB_STATE__RUNNING,
    ))
    ->condition('status_id', $status_id)
    ->execute();
  return $status_id;
}

/**
 * Gets the state of the job.
 * 
 * @param string $data
 *   The payload that identifies the job in question.
 * 
 * @return string
 *   The state of the current job.
 */
function islandora_job_get_job_state($data) {
  $state = db_select('islandora_job_job_status', 'i')
    ->fields('i', array('state'))
    ->condition('data', $data)
    ->execute()
    ->fetchField();

  return $state;
}

/**
 * Gets the status_id of the job.
 * 
 * @param string $data
 *   The payload that identifies the job in question.
 * 
 * @return string
 *   The status_id for the given job.
 */
function islandora_job_get_job_status_id($data) {
  $status_id = db_select('islandora_job_job_status', 'i')
    ->fields('i', array('status_id'))
    ->condition('data', $data)
    ->execute()
    ->fetchField();

  return $status_id;
}

/**
 * Removes a job from tracking.
 *
 * @param string $job_id
 *   The ID of the job to be removed from tracking.
 */
function islandora_job_remove_job_status($job_id) {
  db_delete('islandora_job_job_status')
    ->condition('status_id', $job_id)
    ->execute();
}

/**
 * Ensures there's no duplicate job that already is waiting to be ran.
 *
 * This will ONLY return true if the check for dupes is able to update
 * one row from WAITING to RUNNING.
 *
 * @param string $data
 *   A JSON encoded string representing the payload data.
 *
 * @return bool
 *   TRUE if the job is able to be queued, FALSE otherwise.
 */
function islandora_job_ensure_job_deduping($data) {
  $rows_updated = db_update('islandora_job_job_status')
    ->fields(array(
      'state' => ISLANDORA_JOB_STATE__RUNNING,
    ))
    ->condition('data', $data)
    ->condition('state', ISLANDORA_JOB_STATE__WAITING)
    ->execute();

  if ($rows_updated < 1) {
    // The row might not have existed -- if search for it still yeilds 0
    // records, an attempt to insert the new record here.
    $status_id = islandora_job_get_job_status_id($data);
    if (!$status_id) {
      islandora_job_insert_job_status($data);
      // NOW, when this calls itself again, the "WAITING" record should have existed.
      // there should be a status record...
      return islandora_job_ensure_job_deduping($data);
    }
  }
  return ($rows_updated == 1);
}
