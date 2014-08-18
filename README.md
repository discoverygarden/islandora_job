Islandora Job
=============

Drupal module to facilitate asynchronous and parallel processing of Islandora jobs.  Allows for Drupal modules to register worker functions and routes messages received from a job server to said functions.

Requirements
------------
- A functioning Gearman installation and access to the gearman shell command.

Installation
------------
- Download and install like any other drupal module.
- At least one gearman worker process from the supplied bash script must be running.

Usage
-----
Start a worker process with this:
```bash
$ gearman_worker.sh PATH_TO_DRUPAL_ROOT
```
So long as the rest of the stack is functioning, all Gearman functions of type 'islandora_job' will get handled.
