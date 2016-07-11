/*
 * Created by Eda Yildirim
 *  (2016 JGU)
 *
 *  email:eda.yildirim@cern.ch
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <map>
#include <utility>
#include <algorithm>
using namespace std;

const int millepede_err_id6 = 40; // counting from 0
const int millepede_err_id7 = 46; // counting from 0
const int millepede_err_column = 4; // counting from zero



void readCsvFile();
void readmillepede(TString millepede_res, double errors[]);
double decodeError(string aline);

void calculate_LA_error(vector<int> runnums, double weighted_error[]);

