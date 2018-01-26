<?php

/**
 * @file
 * Job callback for ingesting a batch set.
 */

/**
 * Triggers the ingestion of a batch set via a Gearman worker.
 *
 * @param int $set_id
 *   The ID of the batch set being ingested.
 */
function islandora_job_islandora_batch_ingest_ingest_set($set_id) {
  islandora_batch_set_operations(array('ingest_set' => $set_id));
  drush_backend_batch_process();
}
