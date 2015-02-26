<?php

/**
 * @file
 * Functions for interacting with managed gearman workers.
 */

// Base for automatically created worker pid files.
define('ISLANDORA_JOB_MANAGED_WORKER_PIDFILE_BASE', 'islandora_job_managed_worker_pid_file');

/**
 * Start a managed worker.
 */
function islandora_job_start_managed_worker() {
  module_load_include('inc', 'islandora_job', 'includes/utilities');

  $worker = islandora_job_count_managed_workers() + 1;
  islandora_job_start_worker(
    islandora_job_get_managed_worker_pid_file($worker)
  );
}

/**
 * Stop all managed workers.
 */
function islandora_job_stop_managed_workers() {
  module_load_include('inc', 'islandora_job', 'includes/utilities');

  $number_of_workers = islandora_job_count_managed_workers();
  for ($worker = 0; $worker < $number_of_workers; ++$worker) {
    islandora_job_stop_worker(
      islandora_job_get_managed_worker_pid_file($worker + 1)
    );
  }
}

/**
 * Gets a pid file for a worker number.
 */
function islandora_job_get_managed_worker_pid_file($number) {
  $file_base = ISLANDORA_JOB_MANAGED_WORKER_PIDFILE_BASE;
  return "{$file_base}_$number";
}

/**
 * Count all managed workers.
 */
function islandora_job_count_managed_workers() {
  module_load_include('inc', 'islandora_job', 'includes/utilities');

  $file_base = ISLANDORA_JOB_MANAGED_WORKER_PIDFILE_BASE;
  $files = file_scan_directory(
    'temporary://',
    "/{$file_base}_.*/",
    array('recurse' => FALSE)
  );

  return count($files);
}
