#ifndef F_MSS_H
#define F_MSS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int mss_enum(int *arr, int size);
int mss_enum2(int *arr, int size);
int mss_divconq(int *arr, int l, int h);
int mss_cross(int *arr, int l, int m, int h);
int mss_linear(int *arr, int size);
int max(int a, int b);
int getlines(std::ifstream &fin, std::vector<std::string> &vstr);
void vec2d_int(std::vector <std::vector<int> > &vint, std::vector<std::string> &vstr);
void output(std::string &fname, std::vector <std::vector<int> > &vint, int *vecsize, int **arr);
void input_random(int **arrs_r, int *arr_size, int size);
void experiment(int *arr_size, int size);

#endif /* F_MSS_H */
