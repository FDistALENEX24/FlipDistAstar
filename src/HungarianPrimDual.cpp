
#include "AStarFlipDistance/HungarianPrimDual.hpp"

HungarianPrimDual::HungarianPrimDual(std::vector<std::vector<int>> &_costMatrix) {
    this->dim = std::max(_costMatrix.size(), _costMatrix[0].size());
    this->rows = _costMatrix.size();
    this->cols = _costMatrix[0].size();
    costMatrix=_costMatrix;



    /*for (int w = 0; w < this->dim; w++) {
        if (w < costMatrix.size()) {
            if (costMatrix[w].size() != this->cols) {
                throw std::invalid_argument("Irregular cost matrix");
            }
            for (int j = 0; j < this->cols; j++) {
                if (std::isinf(costMatrix[w][j])) {
                    throw std::invalid_argument("Infinite cost");
                }
                if (std::isnan(costMatrix[w][j])) {
                    throw std::invalid_argument("NaN cost");
                }
            }
            this->costMatrix[w] = std::vector<int>(costMatrix[w].begin(), costMatrix[w].end());
        } else {
            this->costMatrix[w] = std::vector<int>(this->dim, 0.0);
        }
    }*/

    labelByWorker.resize(this->dim, 0.0);
    labelByJob.resize(this->dim, 0.0);
    minSlackWorkerByJob.resize(this->dim, 0);
    minSlackValueByJob.resize(this->dim, 0.0);
    committedWorkers.resize(this->dim, false);
    parentWorkerByCommittedJob.resize(this->dim, 0);
    matchJobByWorker.resize(this->dim, -1);
    matchWorkerByJob.resize(this->dim, -1);
}

void HungarianPrimDual::computeInitialFeasibleSolution() {
    for (int j = 0; j < dim; j++) {
        labelByJob[j] = INT_MAX;
    }
    for (int w = 0; w < dim; w++) {
        for (int j = 0; j < dim; j++) {
            if (costMatrix[w][j] < labelByJob[j]) {
                labelByJob[j] = costMatrix[w][j];
            }
        }
    }
}

void HungarianPrimDual::executePhase() {
    while (true) {
        int minSlackWorker = -1, minSlackJob = -1;
        int minSlackValue = INT_MAX;
        for (int j = 0; j < dim; j++) {
            if (parentWorkerByCommittedJob[j] == -1) {
                if (minSlackValueByJob[j] < minSlackValue) {
                    minSlackValue = minSlackValueByJob[j];
                    minSlackWorker = minSlackWorkerByJob[j];
                    minSlackJob = j;
                }
            }
        }
        if (minSlackValue > 0) {
            updateLabeling(minSlackValue);
        }
        parentWorkerByCommittedJob[minSlackJob] = minSlackWorker;
        if (matchWorkerByJob[minSlackJob] == -1) {
            /*
             * An augmenting path has been found.
             */
            int committedJob = minSlackJob;
            int parentWorker = parentWorkerByCommittedJob[committedJob];
            while (true) {
                int temp = matchJobByWorker[parentWorker];
                match(parentWorker, committedJob);
                committedJob = temp;
                if (committedJob == -1) {
                    break;
                }
                parentWorker = parentWorkerByCommittedJob[committedJob];
            }
            return;
        } else {
            /*
             * Update slack values since we increased the size of the committed
             * workers set.
             */
            int worker = matchWorkerByJob[minSlackJob];
            committedWorkers[worker] = true;
            for (int j = 0; j < dim; j++) {
                if (parentWorkerByCommittedJob[j] == -1) {
                    int slack = costMatrix[worker][j] - labelByWorker[worker] - labelByJob[j];
                    if (minSlackValueByJob[j] > slack) {
                        minSlackValueByJob[j] = slack;
                        minSlackWorkerByJob[j] = worker;
                    }
                }
            }
        }
    }
}

std::vector<int> HungarianPrimDual::execute() {

    computeInitialFeasibleSolution();
    greedyMatch();

    {
        int w = fetchUnmatchedWorker();
        while (w < dim) {
            initializePhase(w);
            executePhase();
            w = fetchUnmatchedWorker();
        }
    }

    std::vector<int> result(matchJobByWorker.begin(), matchJobByWorker.begin() + rows);
    for (int w = 0; w < result.size(); w++) {
        if (result[w] >= cols) {
            result[w] = -1;
        }
    }

    return result;
}


int HungarianPrimDual::fetchUnmatchedWorker() {
    int w;
    for (w = 0; w < dim; w++) {
        if (matchJobByWorker[w] == -1) {
            break;
        }
    }
    return w;
}

void HungarianPrimDual::greedyMatch() {
    for (int w = 0; w < dim; w++) {
        for (int j = 0; j < dim; j++) {
            if (matchJobByWorker[w] == -1 && matchWorkerByJob[j] == -1 &&
                costMatrix[w][j] - labelByWorker[w] - labelByJob[j] == 0) {
                match(w, j);
            }
        }
    }
}

void HungarianPrimDual::initializePhase(int w) {
    std::fill(committedWorkers.begin(), committedWorkers.end(), false);
    std::fill(parentWorkerByCommittedJob.begin(), parentWorkerByCommittedJob.end(), -1);
    committedWorkers[w] = true;
    for (int j = 0; j < dim; j++) {
        minSlackValueByJob[j] = costMatrix[w][j] - labelByWorker[w] - labelByJob[j];
        minSlackWorkerByJob[j] = w;
    }
}

void HungarianPrimDual::match(int w, int j) {
    matchJobByWorker[w] = j;
    matchWorkerByJob[j] = w;
}

void HungarianPrimDual::reduce() {
    for (int w = 0; w < dim; w++) {
        int min = INT_MAX;
        for (int j = 0; j < dim; j++) {
            if (costMatrix[w][j] < min) {
                min = costMatrix[w][j];
            }
        }
        for (int j = 0; j < dim; j++) {
            costMatrix[w][j] -= min;
        }
    }
    std::vector<int> min(dim, INT_MAX);
    for (int w = 0; w < dim; w++) {
        for (int j = 0; j < dim; j++) {
            if (costMatrix[w][j] < min[j]) {
                min[j] = costMatrix[w][j];
            }
        }
    }
    for (int w = 0; w < dim; w++) {
        for (int j = 0; j < dim; j++) {
            costMatrix[w][j] -= min[j];
        }
    }
}

void HungarianPrimDual::updateLabeling(int slack) {
    for (int w = 0; w < dim; w++) {
        if (committedWorkers[w]) {
            labelByWorker[w] += slack;
        }
    }
    for (int j = 0; j < dim; j++) {
        if (parentWorkerByCommittedJob[j] != -1) {
            labelByJob[j] -= slack;
        } else {
            minSlackValueByJob[j] -= slack;
        }
    }
}


int HungarianPrimDual::parent_execute(const std::vector<int>& edges, int number_of_overall_edges) {
    computeInitialFeasibleSolution();
    greedyMatch();

    {
        int w = fetchUnmatchedWorker();
        while (w < dim) {
            initializePhase(w);
            executePhase();
            w = fetchUnmatchedWorker();
        }
    }

    std::vector<int> result(matchJobByWorker.begin(), matchJobByWorker.begin() + rows);
    for (int w = 0; w < result.size(); w++) {
        if (result[w] >= cols) {
            result[w] = -1;
        }
    }

    int value=0;
    for(int i=0;i<result.size();i++){
        value+=costMatrix[i][result[i]];
    }

    parent_labelByJob=std::vector<int>(labelByJob);
    parent_labelByWorker=std::vector<int>(labelByWorker);
    parent_matchJobByWorker=std::vector<int>(matchJobByWorker);
    parent_matchWorkerByJob=std::vector<int>(matchWorkerByJob);
    edges_used_in_cost_matrix.resize(number_of_overall_edges,-1);
    for(int i=0;i<edges.size();i++){
        edges_used_in_cost_matrix[edges[i]]=i;
    }
    return value;
}

int HungarianPrimDual::child_execute(int edge_given_by_parent, const std::vector<int> &new_costs_in_row) {

    labelByWorker=std::vector<int>(parent_labelByWorker);
    labelByJob=std::vector<int>(parent_labelByJob);
    matchJobByWorker=std::vector<int>(parent_matchJobByWorker);
    matchWorkerByJob=std::vector<int>(parent_matchWorkerByJob);

    int row_given_by_parent=edges_used_in_cost_matrix[edge_given_by_parent];


    int worker=row_given_by_parent;
    int job=matchJobByWorker[row_given_by_parent];
    matchWorkerByJob[matchJobByWorker[row_given_by_parent]]=-1;
    matchJobByWorker[row_given_by_parent]=-1;


    std::vector<int> old_cost=costMatrix[row_given_by_parent];
    costMatrix[row_given_by_parent]=new_costs_in_row;


    int mini=INT_MAX;
    for(int i=0;i<costMatrix[row_given_by_parent].size();i++){
        if(costMatrix[row_given_by_parent][i]-labelByJob[i]<mini){
            mini=costMatrix[row_given_by_parent][i]-labelByJob[i];
        }
    }
    labelByWorker[row_given_by_parent]=mini;






    {
        int alpha= costMatrix[worker][job]-labelByWorker[worker]-labelByJob[job];
        //std::cout<<alpha<<std::endl;
        if (alpha==0){
            match(worker,job);
        }
    }


    {
        int w = fetchUnmatchedWorker();
        while (w < dim) {
            initializePhase(w);
            executePhase();
            w = fetchUnmatchedWorker();
        }
    }



    int value=0;
    for(int i=0;i<matchJobByWorker.size();i++){
        value+=costMatrix[i][matchJobByWorker[i]];
    }

    costMatrix[row_given_by_parent]=old_cost;
    return value;
}

