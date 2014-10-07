Islandora Job
=============

Drupal module to facilitate asynchronous and parallel processing of Islandora jobs.  Allows for Drupal modules to register worker functions and routes messages received from a job server to said functions.

Requirements
------------
- A functioning Gearman installation, access to the gearman shell command, and the Gearman PHP extension.

Installation
------------
- Install Gearman
- Install Gearman PHP Extension
- Download and install like any other drupal module.

Persistent Queue
----------------
By default, the job queue is stored in memory, which is fine for development purposes.  For production environments, you’ll probably want the queue of jobs to survive a crash or system restart. To enable this, you need to pass the `--queue-type` argument to the `gearmand` command to specify which type of persistent store you would like to use.  In addition to the `--queue-type` argument, there are several other optional arguments that must be provided depending on which type of store you choose.  Here are examples for the two database types that Drupal supports.

For MySQL, edit /etc/default/gearman-job-server and add the following arguments to the PARAMS variable:
```bash
--queue-type=MySQL  --mysql-host=localhost  --mysql-port=3306 --mysql-user=drupal --mysql-password=your_pw --mysql-db=drupal --mysql-table=gearman_queue
```

For Postgres, edit /etc/default/gearman-job-server to include:
```bash
export PGHOST=TheHostname
export PGPORT=5432
export PGUSER=drupal
export PGPASSWORD=ThePassword
export PGDATABASE=drupal
PARAMS=”--verbose -q libpq --libpq-table=gearmanqueue1 --verbose”
```

There is no need to manually create any tables for either database type.  Gearman will create the queue table on its own if it does not exist.

Accepting Remote Connections
----------------------------
By default, Gearman will only accept connections from localhost.  If you want to allow connections from other computers,  change the `--listen` argument to include your whitelisted ip in /etc/default/gearman-job-server.
