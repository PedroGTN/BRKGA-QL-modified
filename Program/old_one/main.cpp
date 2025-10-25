#include "BRKGA_QL.h"


// /************************************************************************************
// 								MAIN FUNCTION AREA
// *************************************************************************************/
int main() 
{ 
    // file with test instances and input data
	FILE *arqProblems; 
    arqProblems = fopen ("testScenario.csv", "r"); 

    if (arqProblems == NULL){
        printf("\nERROR: File testScenario.csv not found\n");
        getchar();
        exit(1);
    }

    char nameTable[100];

    //read first line of arqProblems file
    if (fgets(nameTable, sizeof(nameTable), arqProblems) == NULL) {
        printf("\nERROR: File %s not found\n", nameTable);
        getchar();
        exit(1);
    }
    
    // best solution that is saved in out file
    TSol sBest;
    sBest.ofv = INFINITY;
    sBest.flag = 0;
    sBest.label = 0;
    sBest.promising =0;
    sBest.similar = 0;

	// run the BRKGA-QL for all test instances
	while (!feof(arqProblems))
	{
		// read the name of instances, debug mode, local search module, maximum time, maximum number of runs, maximum number of threads
        if (fscanf(arqProblems,"%s %d %d %d %d %d %d %f", nameTable, &debug, &numDecoders, &numLS, &MAXTIME, &MAXRUNS, &MAX_THREADS, &OPTIMAL) == 0) {
            printf("\nERROR: File %s not found\n", nameTable);
            getchar();
            exit(1);
        }
		
        strcpy(instance,nameTable);
        
		//read data of the instance
        ReadData(nameTable, n);

        double foBest = INFINITY,
               foAverage = 0;

        float timeBest = 0,
              timeTotal = 0;

        std::vector <double> ofvs;
        ofvs.clear();

        // best solutions found in MAXRUNS
        sBest.ofv = INFINITY;

		// run BRKGA-QL MaxRuns for each instance
        printf("\n\nInstance: %s \nRun: ", instance);
        for (int j=0; j<MAXRUNS; j++)
        {
            // fixed seed
            if (debug == 1)
                srand(j+1); 
            else
                srand(time(NULL));

            printf("%d ", j+1);
            
            gettimeofday(&Tstart, NULL);
            gettimeofday(&Tend, NULL);
            gettimeofday(&Tbest, NULL);

            // best solution found in this run
            bestSolution.ofv = INFINITY;

            // execute the evolutionary method
            BRKGA_QL();

            gettimeofday(&Tend, NULL);

            // store the best solution found in MAXRUNS
            if (bestSolution.ofv < sBest.ofv)
                sBest = bestSolution;

            // calculate best and average results
            if (bestSolution.ofv < foBest)
                foBest = bestSolution.ofv;

            foAverage += bestSolution.ofv;

            // fitness of each solution found in the runs
            ofvs.push_back(bestSolution.ofv);

            timeBest += ((Tbest.tv_sec  - Tstart.tv_sec) * 1000000u + Tbest.tv_usec - Tstart.tv_usec) / 1.e6;
            timeTotal += ((Tend.tv_sec  - Tstart.tv_sec) * 1000000u + Tend.tv_usec - Tstart.tv_usec) / 1.e6; 
        }

        // create a .csv file with average results
        foAverage = foAverage / MAXRUNS;
        timeBest = timeBest / MAXRUNS;
        timeTotal = timeTotal / MAXRUNS;

        if (!debug)
        {
        	WriteSolution(sBest, n, timeBest, timeTotal, instance);
        	WriteResults(foBest, foAverage, ofvs, timeBest, timeTotal, instance);
        }
        else
        {
            WriteSolutionScreen(sBest, n, timeBest, timeTotal, instance);
        }

        // free memory with problem data
        FreeMemoryProblem();

        // free memory of BRKGA-QL components
        FreeMemory();
    }

    fclose(arqProblems);
    return 0;
}