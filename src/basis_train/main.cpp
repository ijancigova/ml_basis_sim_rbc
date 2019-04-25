#include <stdio.h>
#include <basis_experiment.h>
#include <thread>

void experiment_3()
{
    BasisExperiment  basis_experiment( "dataset_params/dataset_3.json",
                                       "/home/michal/cells_result/basis/experiment_3/");

    basis_experiment.learn();

    printf("processing trajectory\n");
    basis_experiment.process();


    printf("processing trajectory errors\n");
    basis_experiment.process_local_prediction_error();


    printf("predict_on_random_seed\n");
    basis_experiment.predict_on_random_seed("dataset_params/dataset_3_seed_c.json");

    printf("saving basis\n");
    basis_experiment.save_basis();
}

void experiment_3_swap()
{
    BasisExperiment  basis_experiment( "dataset_params/dataset_3_swap.json",
                                       "/home/michal/cells_result/basis/experiment_3_swap/");

    basis_experiment.learn();

    printf("processing trajectory\n");
    basis_experiment.process();

    printf("processing trajectory errors\n");
    basis_experiment.process_local_prediction_error();

    printf("saving basis\n");
    basis_experiment.save_basis();
}

void experiment_3_verification()
{
    BasisExperiment  basis_experiment( "dataset_params/dataset_3_verification.json",
                                       "/home/michal/cells_result/basis/experiment_3_verification/");

    basis_experiment.learn();

    printf("processing trajectory\n");
    basis_experiment.process();

    printf("processing trajectory errors\n");
    basis_experiment.process_local_prediction_error();

    printf("saving basis\n");
    basis_experiment.save_basis();
}

void experiment_3_verification_swap()
{
    BasisExperiment  basis_experiment( "dataset_params/dataset_3_verification_swap.json",
                                       "/home/michal/cells_result/basis/experiment_3_verification_swap/");

    basis_experiment.learn();

    printf("processing trajectory\n");
    basis_experiment.process();

    printf("processing trajectory errors\n");
    basis_experiment.process_local_prediction_error();

    printf("saving basis\n");
    basis_experiment.save_basis();
}


void experiment_4()
{
  BasisExperiment  basis_experiment( "dataset_params/dataset_4.json",
                                     "/home/michal/cells_result/basis/experiment_4/");

  basis_experiment.learn();

  printf("processing trajectory\n");
  basis_experiment.process();

  printf("processing trajectory errors\n");
  basis_experiment.process_local_prediction_error();

  printf("saving basis\n");
  basis_experiment.save_basis();
}

void experiment_4_swap()
{
    BasisExperiment  basis_experiment( "dataset_params/dataset_4_swap.json",
                                       "/home/michal/cells_result/basis/experiment_4_swap/");

    basis_experiment.learn();

    printf("processing trajectory\n");
    basis_experiment.process();

    printf("processing trajectory errors\n");
    basis_experiment.process_local_prediction_error();

    printf("saving basis\n");
    basis_experiment.save_basis();
}

void experiment_4_verification()
{
    BasisExperiment  basis_experiment( "dataset_params/dataset_4_verification.json",
                                       "/home/michal/cells_result/basis/experiment_4_verification/");

    basis_experiment.learn();

    printf("processing trajectory\n");
    basis_experiment.process();

    printf("processing trajectory errors\n");
    basis_experiment.process_local_prediction_error();

    printf("saving basis\n");
    basis_experiment.save_basis();
}

void experiment_4_verification_swap()
{
    BasisExperiment  basis_experiment( "dataset_params/dataset_4_verification_swap.json",
                                       "/home/michal/cells_result/basis/experiment_4_verification_swap/");

    basis_experiment.learn();

    printf("processing trajectory\n");
    basis_experiment.process();

    printf("processing trajectory errors\n");
    basis_experiment.process_local_prediction_error();

    printf("saving basis\n");
    basis_experiment.save_basis();
}


void experiment_5()
{
  BasisExperiment  basis_experiment( "dataset_params/dataset_5.json",
                                     "/home/michal/cells_result/basis/experiment_5/");

  basis_experiment.learn();

  printf("processing trajectory\n");
  basis_experiment.process();

  printf("processing trajectory errors\n");
  basis_experiment.process_local_prediction_error();

  printf("saving basis\n");
  basis_experiment.save_basis();
}

int main()
{
  srand(time(NULL));

  experiment_3();

  experiment_3_swap();
  experiment_3_verification();
  experiment_3_verification_swap();

  experiment_4();
  experiment_4_swap();
  experiment_4_verification();
  experiment_4_verification_swap();

  experiment_5();

/*
  std::thread t_experiment_0(experiment_3);
  std::thread t_experiment_1(experiment_3_swap);
  std::thread t_experiment_2(experiment_3_verification);
  std::thread t_experiment_3(experiment_3_verification_swap);

  std::thread t_experiment_4(experiment_4);
  std::thread t_experiment_5(experiment_4_swap);
  std::thread t_experiment_6(experiment_4_verification);
  std::thread t_experiment_7(experiment_4_verification_swap);

  std::thread t_experiment_8(experiment_5);

  t_experiment_0.join();
  t_experiment_1.join();
  t_experiment_2.join();
  t_experiment_3.join();
  t_experiment_4.join();
  t_experiment_5.join();
  t_experiment_6.join();
  t_experiment_7.join();
  t_experiment_8.join();
*/

  printf("program done\n");
  return 0;
}
