
# Omar Pasalic's IntroPARCO first deliverable: OpenMP parallelization

To reproduce the results given in the paper, one must follow the given steps below:

- Login into the GlobalConnect VPN using your own university credentials;
- SSH into the UniTN cluster via _ssh name.surname@hpc.unitn.it_;
    - Linux/OS X users can use the default terminal emulator, while Windows users must download an SSH client like _PuTTY_ or _Mobaxterm_.
- Clone the repository and run the job:

    ```
    git clone https://github.com/OmarPP03/IntroPARCOH1.git
    cd IntroPARCOH1
    qsub -I job.pbs
    ```
- Compile and run the code:

    ```
    cd IntroPARCOH1
    module load gcc91
    gcc-9.1.0 -o exec main.c ppc.c -mavx2 -fopenmp
    ./exec
    ```

