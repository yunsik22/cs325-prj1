#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <unistd.h>
#include "mss.h"


int main(int argc, char **argv) {
    std::string fin_name, fout_name;
    std::cout << "Enter input file name: ";
    getline(std::cin, fin_name);
    std::cin.clear();
    //std::cin.ignore(10000, '\n');
    
    std::ifstream f;
    f.open(fin_name.c_str());
    if (!f.good()) {
        std::cout << fin_name << " does not exist. exiting..." << std::endl;
        return 0;
    }
    
    fout_name = fin_name.substr(0, fin_name.length() - 4);
    fout_name += "_out.txt";
    
    std::vector<std::string> v_str;
    int nlns = getlines(f, v_str);
    std::vector <std::vector<int> > v_int;
    v_int.reserve(nlns);
    vec2d_int(v_int, v_str);
    
    int **results = new int*[nlns];
    for (int i = 0; i < nlns; ++i)
        results[i] = new int[4];
    
    for (int i = 0; i < nlns; ++i) {
        int size = v_int[i].size();
        int *arr = new int[size];
        for (int j = 0; j < size; ++j)
            arr[j] = v_int[i][j];
        
        *results[i] = mss_enum(arr, size);
        *(results[i] + 1) = mss_enum2(arr, size);
        *(results[i] + 2) = mss_divconq(arr, 0, size - 1);
        *(results[i] + 3) = mss_linear(arr, size);
        delete[] arr;
    }
    
    output(fout_name, v_int, &nlns, results);
    
    for (int i = 0; i < nlns; ++i) delete[] results[i];
    delete[] results;
    
    
    // experimental analysis
    int arr_size[] = {100, 200, 400, 600, 800, 1000};
    experiment(arr_size, sizeof(arr_size) / sizeof(arr_size[0]));

    return 0;
}

void experiment(int *arr_size, int size) {
    int **input_r = new int*[size];
    input_random(input_r, arr_size, size);
    
    int **results = new int*[size];
    float **durs = new float*[size];
        
    clock_t t;
    for (int i = 0; i < size; ++i) {
        results[i] = new int[4];
        durs[i] = new float[4];
        
        t = clock();    
        results[i][0] = mss_enum(input_r[i], arr_size[i]);
        t = clock() - t;
		durs[i][0] = (float)t / CLOCKS_PER_SEC;
        sleep(2); // make the programme waiting for 2 secondes
        t = clock();    
        results[i][1] = mss_enum2(input_r[i], arr_size[i]);
        t = clock() - t;
		durs[i][1] = (float)t / CLOCKS_PER_SEC;
        sleep(2);
        t = clock();    
        results[i][2] = mss_divconq(input_r[i], 0, arr_size[i] - 1);
        t = clock() - t;
		durs[i][2] = (float)t / CLOCKS_PER_SEC;
        sleep(2);
        t = clock();    
        results[i][3] = mss_linear(input_r[i], arr_size[i]);
        t = clock() - t;
		durs[i][3] = (float)t / CLOCKS_PER_SEC;
        sleep(2);
	std::cout << "move onto next algorithm..." << std::endl;
    }
    
    // output the results
    std::ofstream fout;
    char fname[] = "experiment_out.txt";
    fout.open(fname);
    for (int i = 0; i < size; ++i) {
        fout << "Input size: " << arr_size[i] << "\n";
        fout << "mss_enum   => max sum: " << results[i][0] << " | duration: " << durs[i][0] << "\n";
        fout << "mss_enum2  => max sum: " << results[i][1] << " | duration: " << durs[i][1] << "\n";
        fout << "mss_divcq  => max sum: " << results[i][2] << " | duration: " << durs[i][2] << "\n";
        fout << "mss_linear => max sum: " << results[i][3] << " | duration: " << durs[i][3] << "\n";
        fout << "*****************************************************************************" << "\n";
    }
    fout.close();
    std::cout << fname << " created" << std::endl;
    
    for (int i = 0; i < size; ++i) {
        delete[] results[i];
        delete[] durs[i];
        delete[] input_r[i];
    }
    delete[] results;
    delete[] durs;
    delete[] input_r;
}


void input_random(int **arrs_r, int *arr_size, int size) {
    /* initialize random seed */
    srand(time(NULL));
    for (int i = 0; i < size; ++i) {
        arrs_r[i] = new int[arr_size[i]];
        for (int j = 0; j < arr_size[i]; ++j) {
            /* generate secret number between 0 and 100: */
            arrs_r[i][j] = rand() % 100;
            /* generate secret number between 0 and 1: */
            if (rand() % 2) arrs_r[i][j] *= -1;
        }
    }
}

void output(std::string &fname, std::vector <std::vector<int> > &vint, int *vecsize, int **arr) {
    std::ofstream fout;
    fout.open(fname.c_str());
    int size;
    
    for (int i = 0; i < *vecsize; ++i) {
        fout << "[";
        size = vint[i].size();
        for (int j = 0; j < size - 2; ++j)
            fout << vint[i][j] << ", ";
        fout << vint[i][size - 2] << "]" << std::endl;

        fout << "maxsubarr_enum = " << arr[i][0] << std::endl;
        fout << "maxsubarr_enum2 = " << arr[i][1] << std::endl;
        fout << "maxsubarr_divconq = " << arr[i][2] << std::endl;
        fout << "maxsubarr_linear = " << arr[i][3] << std::endl;
        fout << std::endl;
    }
    fout.close();
    std::cout << fname << " created" << std::endl;
}

void vec2d_int(std::vector <std::vector<int> > &vint, std::vector<std::string> &vstr) {
    int size = vstr.size();
    char *cstr, *p;
    for (int i = 0; i < size; ++i) {
        std::vector<int> row; // create an empty row
        
        cstr = new char[vstr[i].length() + 1];
        strcpy(cstr, vstr[i].c_str());
        p = strtok(cstr, " ,[]");
        
        while (p) {
            //printf("[%s]", p);
            row.push_back(atoi(p));
            p = strtok(NULL, " ,[]");
        }
        
        delete[] cstr;
        vint.push_back(row);
        //std::cout << std::endl;
    }
}


int getlines(std::ifstream &fin, std::vector<std::string> &vec) {
    std::string ln;
    if (fin.is_open()) {
        while (getline(fin, ln)) {
            vec.push_back(ln);
        }
    fin.close();
    }
    return vec.size();
}



/* O(n) */
int mss_linear(int *arr, int size) {
    int max, tmpsum;
    max = INT_MIN;
    tmpsum = 0;
    for (int i = 0; i < size; ++i) {
        tmpsum += arr[i];
        if (tmpsum > 0) {
            if (tmpsum > max)
                max = tmpsum;
        }
        else
            tmpsum = 0;
    }
    return max;
}


/* O(n^3) */
int mss_enum(int *arr, int size) {
    int max, sum;
    max = INT_MIN;
    for (int i = 0; i < size; ++i) {
        for (int j = i; j < size; ++j) {
            sum = 0;
            for (int k = i; k <= j; ++k) {
                sum += arr[k];
            }
            max = (sum > max) ? sum : max;
        }
    }
    return max;
}


/* O(n^2) */
int mss_enum2(int *arr, int size) {
    int max, sum;
    max = INT_MIN;
    for (int i = 0; i < size; ++i) {
        sum = 0;
        for (int j = i; j < size; ++j) {
            sum += arr[j];
            max = (sum > max) ? sum : max;
        }
    }
    return max;
}


/* O(nlgn)
 * T(n) = 2T(n/2) + n
 * a = 2, b = 2, f(n) = n  -> case #2 -> f(n)*lgn
*/
int mss_divconq(int *arr, int l, int h) {
    if (l == h)
        return arr[l];
    int m = (l + h) / 2;
    int max_left = mss_divconq(arr, l, m);
    int max_right = mss_divconq(arr, m + 1, h);
    int max_cross = mss_cross(arr, l, m, h);
    return max(max(max_left, max_right), max_cross);
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int mss_cross(int *arr, int l, int m, int h) {
    int sum_left, sum_right, sum;
    sum_left = sum_right = INT_MIN;
    sum = 0;
    for (int i = m; i >= l; --i) {
        sum += arr[i];
        sum_left = (sum > sum_left) ? sum : sum_left;
    }
    sum = 0;
    for (int i = m + 1; i <= h; ++i) {
        sum += arr[i];
        sum_right = (sum > sum_right) ? sum : sum_right;
    }
    return sum_left + sum_right;
}
