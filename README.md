# Islandora Job

Drupal module to facilitate asynchronous and parallel processing of Islandora jobs.  Allows for Drupal modules to register worker functions and routes messages received from a job server to said functions.

## Requirements
- A functioning Gearman installation, access to the gearman shell command, and the Gearman PHP extension.

## Install Gearman Server (Ubuntu 14.04)
- `apt-get install libgearman-dev gearman-job-server gearman-tools`
- In your favorite text editor update the parameters to be passed to Gearman in ` /etc/default/gearman-job-server`. In this example Gearman manages itself on a local database as outlined [below](#persistent-database-queue).
     * `PARAMS="--listen=127.0.0.1 --queue-type=MySQL --mysql-host=localhost --mysql-port=3306 --mysql-user=$DRUPAL_USERNAME --mysql-password=$DRUPAL_PASSWORD --mysql-db=$DRUPAL_DB --mysql-table=gearman_queue"`
- Add an [override](http://upstart.ubuntu.com/cookbook/) in `/etc/init/gearman-job-server.override` such that Gearman starts and stops accordingly.

  ```bash
   ############################
   # -*- upstart -*-
   # Upstart configuration script for "gearman-job-server".
   description "gearman job control server"
   start on (filesystem and net-device-up IFACE=lo)
   stop on runlevel [!2345]
   respawn
   # gearman-job-server need to talk to the DB.
   start on started mysql
   stop on stopping mysql
   # exec start-stop-daemon --start --chuid gearman --exec /usr/sbin/gearmand -- --log-file=/var/log/gearman-job-server/gearman.log
   # PATCH: https://bugs.launchpad.net/ubuntu/+source/gearmand/+bug/1260830
   script
   . /etc/default/gearman-job-server
   exec start-stop-daemon --start --chuid gearman --exec /usr/sbin/gearmand -- $PARAMS --log-file=/var/log/gearman-job-server/gearman.log
   end script
   ############################
   ```
- Follow the instructions to install the [gearman-init](https://github.com/discoverygarden/gearman-init) system service scripts to manage workers.

## Install Gearman PHP Extension
- Install the Gearman PHP extension via PECL
  * `/usr/bin/pecl install gearman`
- Make the extension available for use by PHP
  * `echo extension=gearman.so >> /etc/php5/mods-available/gearman.ini`
- Enable the Gearman PHP library.
  * In PHP versions > 5.4 you can use `php5enmod`
    *  `php5enmod gearman`
  * Otherwise, do the following
    * `ln -s /etc/php5/mods-available/gearman.ini /etc/php5/apache2/conf.d/20-gearman.ini`
    * `ln -s /etc/php5/mods-available/gearman.ini /etc/php5/cli/conf.d/20-gearman.ini`
- Restart the webserver
  * `service apache2 restart`

Download and install the Islandora Job module like any other Drupal module.

## Usage

This module aims to be as simple and ‘Drupal-y’ as possible.  To register a function as a job, your module must implement `hook_register_job()`.  This hook must return an array whose keys are the names of the functions you want to be jobs, and whose values are an associative array modeled after the input to `module_load_include`.

For example, in your .module file:
```php
/**
* Implements hook_register_jobs().
*/
function my_module_register_jobs() {
  $jobs = array(
    'my_module_hello_world' => array(
      'type' => 'inc',                  // The extension of the file that defines 'my_module_example_job'
      'module' => 'my_module',          // The module that has 'my_module_example_job'
      'name' => 'includes/utilities',   // The relative path to the file that defines 'my_module_example_job'.  
                                        // Note the extension is missing just like in module_load_include().
                                        // This argument is not required if your function is in the .module file.
    ),
  );
  return $jobs;
}
```

In your utilities file:
```php
/**
 * Betcha can't guess what this'll do!
 */
function my_module_hello_world($name) {
  $greeting = "Hello $name";
  return $greeting;
}
```

Elsewhere in your code, you can submit jobs in one of three ways: in the foreground, in the foreground as a batch, or in the background.  In all three cases, arguments to job function are provided to the submission function and are automagically passed through to the job function itself!  The only caveat is that arguments that are objects must be serialized, and the job function must know to deserialize them.  See the tests for more examples.

Executing a job in the foreground blocks until complete and returns the result.
```php
islandora_job_submit('my_module_hello_world', "Danny"); // Returns "Hello Danny"
```

Executing a job in the background doesn't block, but only returns the id of the job that was submitted to the server.  The results of the job are never returned to the code the submitted the job.
```php
islandora_job_submit_background('my_module_hello_world', "Jordan"); // Returns something like "H:your-machine-name:90"
```

Executing a jobs as a foreground batch will block until all jobs have executed and returns TRUE if there were no errors, otherwise FALSE.
```php
$batch = array(
  array(
    'job_name' => 'my_module_hello_world',
    'args' => array(
      'Will',
    )
  ),
  array(
    'job_name' => 'my_module_hello_world',
    'args' => array(
      'Adam',
    )
  ),
  array(
    'job_name' => 'my_module_hello_world',
    'args' => array(
      'Morgan',
    )
  ),
  array(
    'job_name' => 'my_module_hello_world',
    'args' => array(
      'Alan',
    )
  ),
  array(
    'job_name' => 'my_module_hello_world',
    'args' => array(
      'Matt',
    )
  ),
);

islandora_job_submit_batch($batch);  // Returns TRUE
```

## Starting and Stopping Workers

Workers are started and stopped via system services such as those implemented in [gearman-init](https://github.com/discoverygarden/gearman-init). If you define a new job by adding a new entry to your module’s implementation of hook_register_jobs(), you’ll have to restart all workers for the jobs to become registered and available for the workers.

## Persistent Database Queue

By default, the job queue is stored in memory, which is fine for development purposes.  For production environments, you’ll probably want the queue of jobs to survive a crash or system restart. To enable this, you need to pass the `--queue-type` argument to the `gearmand` command to specify which type of persistent store you would like to use.  In addition to the `--queue-type` argument, there are several other optional arguments that must be provided depending on which type of store you choose.  Here are examples for the two database types that Drupal supports.

For MySQL (available since 0.34), edit /etc/default/gearman-job-server and add the following arguments to the PARAMS variable:
```bash
--queue-type=MySQL --mysql-host=localhost --mysql-port=3306 --mysql-user=drupal_db_user --mysql-password=drupal_db_pw --mysql-db=drupal --mysql-table=gearman_queue
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

Of course, you’ll have to restart the server for these changes to take effect.
```bash
sudo service restart gearman-job-server
```

## How many jobs are left?

You can determine how many job are left by issuing a query to the database table defined in the `gearmand` args.

```sql
SELECT function_name, COUNT(function_name) FROM gearman_queue GROUP BY function_name;
```

This, of course, assumes you are using a database for persistence.

## Clearing the job queue

Let's face it, at some point in time you're going to screw something up and flood the job server with a bunch of jobs that are going to error 'til the cows come home.  You can use SQL to delete jobs from the queue table, or just truncate the table entirely.

```sql
# For a single job type
DELETE FROM gearman_queue WHERE function_name = "my_borked_function_name";

# Nuke 'em Rico!
TRUNCATE TABLE gearman_queue;
```

This, of course, assumes you are using a database for persistence.  If you like running fast and loose (e.g. no persistence), then you can just restart the gearman server and it’ll clobber any jobs remaining.
```bash
sudo service restart gearman-job-server
```

## Accepting Remote Connections

By the defaults defined in /etc/default/gearman-job-server, Gearman will only accept connections from localhost.  If you want to allow connections from other computers,  change the `--listen` argument to include your whitelisted ip in the defaults file.
