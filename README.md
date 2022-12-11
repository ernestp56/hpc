# Environment setup

## Installation guide
Required plugins:
- Visual Studio Code: Remote - SSH

Connect to the SSH using Uni credentials, remember to use VPN

Afterwards press Open folder, press enter

## Cloning the Git repo to your login node on the cluster

You'll need to generate an ssh key on your cluster
<code>ssh-keygen</code>

Then copy it
<code>ssh-keygen</code>
cat ~/.ssh/id_rsa.pub

And afterwards you need to add it in your Github Settings, as a new SSH key.
Clone the Git repo to your login node.

Happy committing :)

## The process

First, load the mpi on your cluster
<code>module load mpich-3.2</code>

Second, create executable from your c file (the last file is the source)
<code>mpicc -g -Wall -o mpi-hello-world mpi-hello-world.c</code>

Third, create a shell script that will allocate the memory, etc. and execute your executable on the cluster, e.g. test.sh

Then run qsub submit the shell script.
Afterwards, you can monitor using qstat.
After the job is done, run
<code>more jobId</code>

If you don't specify queue, it goes to the short queue

## Commands cheat sheet

Promote files from local to the cluster (no longer needed if you use the git repo on login node)
<code>scp program-output ernest.perkowski@hpc.unitn.it:/home/ernest.perkowski</code>

Build an executable on the cluster
<code>mpicc -g -Wall -o mpi-hello-world mpi-hello-world.c</code>

Build an executable (locally)
<code>gcc program.c -c program-output</code>

Submit a job on the cluster
<code>qsub program-output</code>

Check the job status on the cluster
<code>qstat jobId</code>
