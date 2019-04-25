#include "trajectory_experiment.h"

#include <timer.h>
#include <math.h>

TrajectoryExperiment::TrajectoryExperiment(std::string parameters_file_name, DatasetInterface *dataset)
{
  JsonConfig json(parameters_file_name);
  this->dataset = dataset;

  parameters = json.result;

  init();
}


void TrajectoryExperiment::init()
{
  log_prefix = parameters["experiment_log_prefix"].asString();

  experiment_log.set_output_file_name(log_prefix + "experiment.log");
  training_log.set_output_file_name(log_prefix + "training_progress.log");

  experiment_log << "initializing\n";

  input_geometry.w = dataset->get_width();
  input_geometry.h = dataset->get_height();
  input_geometry.d = dataset->get_channels();

  output_geometry.w = 1;
  output_geometry.h = 1;
  output_geometry.d = dataset->get_output_size();


  unsigned int batch_max_size  = parameters["batch_max_size"].asInt();

  batch = new Batch(input_geometry,
                    output_geometry,
                    batch_max_size);


  experiment_log << "creating batch with size " << batch->size() << "\n";

  fill_batch();


  init_networks_try_count   = parameters["init_networks_try_count"].asInt();
  epoch_count               = parameters["epoch_count"].asInt();
  learning_rate_decay       = parameters["learning_rate_decay"].asFloat();

  learning_rate       = parameters["network_architecture"]["hyperparameters"]["learning_rate"].asFloat();
  lambda              = parameters["network_architecture"]["hyperparameters"]["lambda"].asFloat();

  experiment_log << "\n";
  experiment_log << "parameters :\n";

  experiment_log << "init networks try count : " << init_networks_try_count << "\n";
  experiment_log << "epoch count :             " << epoch_count << "\n";

  experiment_log << "learning rate_decay : " << learning_rate_decay << "\n";

  experiment_log << "init done\n\n";
}

TrajectoryExperiment::~TrajectoryExperiment()
{
  experiment_log << "uninit\n";

  if (batch != nullptr)
  {
    delete batch;
    batch = nullptr;
  }

  experiment_log << "uninit done\n";
}



void TrajectoryExperiment::run()
{
  srand(time(NULL));

  error_best = 1000000000.0;

  //skip search init network when there is pretrained net
  if (parameters["use_pretrained"].asBool() == false)
  {
    if (search_initial_net() != 0)
      return;
  }


  std::string best_net_file_name;
  best_net_file_name = log_prefix + "trained/";

  experiment_log << "loading from : " << best_net_file_name << "cnn_config.json" << "\n";
  CNN *nn = new CNN(best_net_file_name + "cnn_config.json", input_geometry, output_geometry);

  learning_rate = nn->get_learning_rate();
  lambda        = nn->get_lambda();

  epoch_without_improvement = 0;

  experiment_log << "training\n\n";


  //learn network -> use best network
  for (unsigned int epoch = 0; epoch < epoch_count; epoch++)
  {
    experiment_log << "epoch " << epoch << "\n";

    fill_batch();

    //learning
    for (unsigned int i = 0; i < batch->size(); i++)
    {
      batch->set_random();
      nn->learn(batch->get_output(), batch->get_input());

      if ((i%(batch->size()/100)) == 0)
      {
        sTrajectoryTestResult quick_result = test(nn, true);

        float done = (100.0*i)/batch->size();

        experiment_log << "training done = " << done << " ";

        experiment_log << "dif_error = ";
        experiment_log << quick_result.dif_error << " ";
        experiment_log << quick_result.dif_error_min << " ";
        experiment_log << quick_result.dif_error_max << " ";

        experiment_log << "   nn_error = ";

        experiment_log << quick_result.nn_error << " ";
        experiment_log << quick_result.nn_error_min << " ";
        experiment_log << quick_result.nn_error_max;
        experiment_log << "\n";
      }
    }

    //process testing on testing set
    experiment_log << "testing\n";
    result = test(nn);

    //NaN error -> set lower learning rate and load best network
    if (result.output_valid == false)
    {
      delete nn;
      nn = load_saved_net(best_net_file_name);
    }
    //error too big -> set lower learning rate and load best network
    else if (result.too_big_difference)
    {
      delete nn;
      nn = load_saved_net(best_net_file_name);
    }
    //best network -> save result
    else if (result.best_net)
    {
      nn->save(best_net_file_name);

      experiment_log << " error = " << result.nn_error << " saved as new best network\n";

      epoch_without_improvement = 0;

      best_result = result;

      best_net_log_process();

      save_examples(nn);
    }
    else if (result.long_term_without_improvement)
    {
      delete nn;
      nn = load_saved_net(best_net_file_name);
    }

    experiment_log << "dif_error = ";
    experiment_log << result.dif_error << " ";
    experiment_log << result.dif_error_min << " ";
    experiment_log << result.dif_error_max << " ";

    experiment_log << "   nn_error = ";

    experiment_log << result.nn_error << " ";
    experiment_log << result.nn_error_min << " ";
    experiment_log << result.nn_error_max;
    experiment_log << "\n\n";


    training_log << epoch << " ";
    training_log << result.dif_error << " ";
    training_log << result.dif_error_min << " ";
    training_log << result.dif_error_max << " ";

    training_log << result.nn_error << " ";
    training_log << result.nn_error_min << " ";
    training_log << result.nn_error_max << "\n";
  }

  delete nn;
  experiment_log << "training done\n";
}




int TrajectoryExperiment::search_initial_net()
{
  bool saved = false;

  std::string best_net_file_name;
  best_net_file_name = log_prefix + "trained/";

  experiment_log << "search initial network\n\n";

  //try learn few networks one epoch
  for (unsigned int nn_try = 0; nn_try < init_networks_try_count; nn_try++)
  {
    experiment_log << "creating new net with id " << nn_try << "\n";

    CNN *nn = new CNN(parameters["network_architecture"], input_geometry, output_geometry);

    experiment_log << "training network\n";

    //learning
    for (unsigned int i = 0; i < batch->size(); i++)
    {
      batch->set_random();
      nn->learn(batch->get_output(), batch->get_input());

      if ((i%(batch->size()/100)) == 0)
      {
        sTrajectoryTestResult quick_result = test(nn, true);

        float done = (100.0*i)/batch->size();

        experiment_log << "training done = " << done << " ";

        experiment_log << "dif_error = ";
        experiment_log << quick_result.dif_error << " ";
        experiment_log << quick_result.dif_error_min << " ";
        experiment_log << quick_result.dif_error_max << " ";

        experiment_log << "   nn_error = ";

        experiment_log << quick_result.nn_error << " ";
        experiment_log << quick_result.nn_error_min << " ";
        experiment_log << quick_result.nn_error_max;
        experiment_log << "\n";
      }
    }

    experiment_log << "testing network\n";

    //process testing on testing set
    sTrajectoryTestResult compare_result = test(nn);

    if (compare_result.output_valid)
    {
      if (compare_result.best_net)
      {
        nn->save(best_net_file_name);

        best_result = result;
        best_net_log_process();

        save_examples(nn);

        saved = true;
        experiment_log << "net " << nn_try << " error = " << compare_result.nn_error << " saved as new best network\n";
      }
      else
        experiment_log << "net " << nn_try << " error = " << compare_result.nn_error << "\n";
    }
    else
    {
      experiment_log << "net " << nn_try << " " << "NaN\n";
    }

    delete nn;

    experiment_log << "\n\n\n";
  }

  //all nets ended as NaN -> terminate experiment, try lower learning rate or initial weights range
  if (saved == false)
  {
    experiment_log << "ending experiment -> no valid network\n";
    return -1;
  }

  return 0;
}

CNN* TrajectoryExperiment::load_saved_net(std::string best_net_file_name)
{
  learning_rate*= learning_rate_decay;
  lambda*= learning_rate_decay;

  if (result.output_valid != true)
    experiment_log << "NaN error, loading best network, and setting learning rate = " << learning_rate << "\n";
  else if (result.too_big_difference)
    experiment_log << "too big error " << result.nn_error <<" , loading best network and setting learning rate = " << learning_rate << "\n";
  else if (result.long_term_without_improvement)
    experiment_log << "long term without improvement, loading best network, and setting learning rate = " << learning_rate << "\n";
  else
    experiment_log << "something wrong\n";

  CNN *nn = new CNN(best_net_file_name + "cnn_config.json", input_geometry, output_geometry);
  nn->set_learning_rate(learning_rate);
  nn->set_lambda(lambda);

  epoch_without_improvement = 0;

  return nn;
}


sTrajectoryTestResult TrajectoryExperiment::test(CNN *nn, bool quick_test)
{
  sTrajectoryTestResult result;

  result.output_valid       = true;
  result.too_big_difference = false;
  result.best_net           = false;

  result.dif_error              = 10000000.0;
  result.dif_error_min          = result.dif_error;
  result.dif_error_max          = -result.dif_error;

  result.nn_error              = 10000000.0;
  result.nn_error_min          = result.nn_error;
  result.nn_error_max          = -result.nn_error;

  result.long_term_without_improvement = false;


  std::vector<float> nn_output(dataset->get_output_size());

  float dif_error_sum = 0.0;
  float nn_error_sum = 0.0;

  unsigned int testing_iterations = dataset->get_testing_size();
  if (quick_test)
    testing_iterations/= 10;

  for (unsigned int i = 0; i < testing_iterations; i++)
  {
    sDatasetItem ground_truth;

    if (quick_test)
      ground_truth = dataset->get_random_testing();
    else
      ground_truth = dataset->get_testing(i);

    float dif_prediction_error = dif_prediction(ground_truth.input, ground_truth.output);
    float nn_prediction_error  = nn_prediction(nn, ground_truth.input, ground_truth.output);

    if (check_valid(nn_output) != true)
    {
      result.output_valid = false;
      break;
    }

    if (dif_prediction_error > result.dif_error_max)
      result.dif_error_max = dif_prediction_error;

    if (dif_prediction_error < result.dif_error_min)
      result.dif_error_min = dif_prediction_error;

    dif_error_sum+= dif_prediction_error;

    if (nn_prediction_error > result.nn_error_max)
      result.nn_error_max = nn_prediction_error;

    if (nn_prediction_error < result.nn_error_min)
      result.nn_error_min = nn_prediction_error;

    nn_error_sum+= nn_prediction_error;

  }


  result.dif_error = dif_error_sum/testing_iterations;
  result.nn_error = nn_error_sum/testing_iterations;

  if (quick_test)
    return result;

  if (result.nn_error < error_best)
  {
    result.best_net = true;
    error_best = result.nn_error;
  }
  else
  if (result.nn_error > error_best*1.2)
  {
    result.too_big_difference = true;
  }
  else
  {
    epoch_without_improvement++;
    if ((int)epoch_without_improvement >= parameters["epoch_without_improvement"].asInt())
    {
      epoch_without_improvement = 0.0;
      result.long_term_without_improvement = true;
    }
  }

  return result;
}


float TrajectoryExperiment::dif_prediction(std::vector<float> &input, std::vector<float> &required_output)
{

  unsigned int padding = (dataset->get_width() - 3)/2;
  unsigned int start_first  = padding + padding*dataset->get_width() + 0*dataset->get_width();
  unsigned int start_second = padding + padding*dataset->get_width() + 1*dataset->get_width();


  float x0 = input[start_first + 0];
  float y0 = input[start_first + 1];
  float z0 = input[start_first + 2];

  float x1 = input[start_second + 0];
  float y1 = input[start_second + 1];
  float z1 = input[start_second + 2];

/*
  float x0 = input[0];
  float y0 = input[1];
  float z0 = input[2];

  float x1 = input[3];
  float y1 = input[4];
  float z1 = input[5];
*/
  std::vector<float> prediction(3);

  prediction[0] = x0 + (x0 - x1);
  prediction[1] = y0 + (y0 - y1);
  prediction[2] = z0 + (z0 - z1);

/*
  printf("[%f %f %f]", x0, y0, z0);
  printf("[%f %f %f]", x1, y1, z1);
  printf("[%f %f %f]\n", prediction[0], prediction[1], prediction[2]);
*/

  return rms(required_output, prediction);
}

float TrajectoryExperiment::nn_prediction(CNN *nn, std::vector<float> &input, std::vector<float> &required_output)
{
  std::vector<float> nn_output(dataset->get_output_size());
  nn->forward_vector(nn_output, input);

  float result = rms(required_output, nn_output);
/*
  if (result > 1.0)
  {
    print_vector(required_output);
    printf(">>>> ");
    print_vector(nn_output);
    printf(" >>> %f\n", result);
  }
*/
  return result;
}

bool TrajectoryExperiment::check_valid(std::vector<float> &v)
{
  for (unsigned int i = 0; i < v.size(); i++)
    if (isnan(v[i]))
      return false;

  for (unsigned int i = 0; i < v.size(); i++)
    if (isinf(v[i]))
      return false;

  return true;
}

void TrajectoryExperiment::fill_batch()
{
  for (unsigned int i = 0; i < batch->size(); i++)
  {
    sDatasetItem item = dataset->get_random_training();
    batch->add(item.output, item.input);
  }
}


float TrajectoryExperiment::rms(std::vector<float> &required, std::vector<float> &testing)
{
  float result = 0.0;
  for (unsigned int i = 0; i < required.size(); i++)
  {
    float tmp = required[i] - testing[i];
    result+= tmp*tmp;
  }

  float error = sqrt(result);

/*
  float required_size = 0.0;
  for (unsigned int i = 0; i < required.size(); i++)
    required_size+= required[i]*required[i];
  required_size = sqrt(required_size);


  float error_normalised = error/(0.00000001 + required_size);
*/
  return error;
}


void TrajectoryExperiment::print_vector(std::vector<float> &v)
{
  for (unsigned int i = 0; i < v.size(); i++)
    printf("%6.3f ", v[i]);
}

void TrajectoryExperiment::best_net_log_process()
{

}

void TrajectoryExperiment::save_examples(CNN *nn)
{
  (void)nn;
  /*
  Log output_examples("best_net_output_examples.log");


  std::vector<float> nn_output(dataset->get_output_size());

  for (unsigned int i = 0; i < 100; i++)
  {
    sDatasetItem ground_truth;
    ground_truth = dataset->get_random_testing();
    nn->forward_vector(nn_output, ground_truth.input);

//    output_examples << "\n\n";
//    output_examples << ground_truth.input << "\n";
    output_examples << ground_truth.output << " >> " << nn_output << "\n";
  }
  */
}
