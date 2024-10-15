World.cc
    //evaluation now occurs explicityly in the phenotype

    double World::evaluate()
    {
      printf("World::evalute() -- DON'T USE ME!\n");
      //return compare_to_other(_goal_world);
      return -1;

    }

ODEWorld.cc

vector<float> ODEWorld::evaluate(Tensegrity *ts, int readFromFile, int do_render, int stopOnFlag){
    //  printf("evaluating...\n");
    ts->fprintGenome("curnetwork.net");
    //  ts->printStringLabels();
    cur_tens = ts;
    _use_networks = 0;
    render = do_render;

    reset();
    int result;
    int is_static = 0;
    result = 1;

    vector<float> results;
    results.clear();

    result = makeFromTensegrity(ts,is_static);

    //  ts->findRandomStringLabels();
    //ts->mutateStringLabels();
    //ts->mutateStringLabels();
    //ts->mutateStringLabels();

    //  resetTens();
    resetTens();
    updateStringLabels(ts);
    //resetT3();
    copyWeightsToElements(ts);

    //  ts->printStringLabels();

    //  ts->print();
    //ts->print_graphviz();


    if (result != 0){
        run_tensegrity(stopOnFlag);

        float dt = distanceTraveled(_currCOM);
        printf("distance:%f\n",dt);

        float vol = get_tensegrity_volume();
        results.push_back(0.); // rank
        results.push_back(dt);
    }
    else{
        //      printf("bad tensegrity\n");
        results.push_back(0.); // rank
        results.push_back(0);
    }

    return results;
}