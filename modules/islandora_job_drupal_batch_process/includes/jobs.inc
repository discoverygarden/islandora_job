<?php

/**
 * @file
 * Job callback for processing a Drupal batch.
 */

/**
 * Executes an arbitrary Drupal batch via Drush.
 *
 * @param array $batch
 *   A batch definition to process.
 */
function islandora_job_drupal_batch_process($batch) {
  batch_set($batch);
  drush_backend_batch_process();
}
