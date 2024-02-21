#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include<vector>
#include<filesystem>
#include <random>
#include <bits/stdc++.h>
#include <fcntl.h>

using namespace std;

namespace ns3 {
    
std::string getFileName (const std::string& filePath) {
  std::string path = filePath.substr(filePath.find("scratch"));
  std::filesystem::path p(std::filesystem::absolute(path));
  p = p.lexically_normal();
  std::string fileName = p.string();
  fileName = fileName.substr(0,fileName.find_last_of('.'));
  return fileName;
}

std::string getBaseName(const std::string& filePath) {
  std::string fileName = getFileName(filePath);
  std::string baseName = fileName.substr(fileName.find_last_of('/'));
  return baseName;
}

std::string getCustomFileName(const std::string &filePath, const std::string &name) {
  std::string fileName = getFileName(filePath);
  fileName = fileName.substr(0, fileName.find_last_of('/'));
  fileName =  fileName + "/" + name;
  return fileName;
}

vector<uint32_t> getRepRate(int n, string file, string name) {
  string fileName = getCustomFileName(file, name);
  FILE* fp = freopen(fileName.c_str(), "r", stdin);
  vector<uint32_t> repRates(n);
  for(int i(0);i<n;i++) {
    cin>>repRates[i];
  }
  fclose(fp);
  return repRates;
}

vector<Vector3D> getPV(int n, string file, string name)  {
  string fileName = getCustomFileName(file, name);
  FILE* fp = freopen(fileName.c_str(), "r", stdin);
  vector<Vector3D> pv(n);
  for(int i(0);i<n;i++) {
    cin>>pv[i].x>>pv[i].y>>pv[i].z;
  }
  fclose(fp);
  return pv;
}
vector<double> getStartTimes(int n, string file, string name){
  string fileName = getCustomFileName(file, name);
  FILE* fp = freopen(fileName.c_str(), "r", stdin);
  vector<double> startTimes(n);
  for(int i(0);i<n;i++) {
    cin>>startTimes[i];
  }
  fclose(fp);
  return startTimes;
}

vector<uint32_t> getGenRates(int n, string file, string name) {
  string fileName = getCustomFileName(file, name);
  FILE* fp = freopen(fileName.c_str(), "r", stdin);
  vector<uint32_t> genRates(n);
  for(int i(0);i<n;i++) {
    cin>>genRates[i];
  }

  fclose(fp);
  return genRates;
}

/* Method 1*/
vector<uint32_t> generateData(uint32_t prioRate, uint32_t genRate)
{
  vector<uint32_t> data(genRate, 5);
  vector<bool> marked(genRate, false);
  default_random_engine generator;
  uniform_int_distribution<int> distribution(0, genRate-1);
  uint32_t cnt = 0;
  while(cnt < prioRate) {
    int num = distribution(generator);
    if (marked[num]) continue;
    data[num] = 7;
    marked[num] = true;
    cnt++;
  }
  return data;
}

/* Method 2 */
vector<uint32_t> generateData2(uint32_t prioRate, uint32_t genRate)
{

  int genv = 5;
  int priov = 7;
  if (genRate < prioRate) {
    swap(genRate, prioRate);
    swap(genv, priov);
  }
  vector<bool> marked(genRate, false);
  vector<uint32_t> prioNums;
  default_random_engine generator;
  uniform_int_distribution<uint32_t> distribution(0, genRate-1);
  uint32_t cnt = 0;
  while(cnt < prioRate)
  {
    uint32_t num = distribution(generator);
    if (marked[num]) continue;
    prioNums.push_back(num);
    cnt++;
  }
  sort(prioNums.begin(), prioNums.end());
  vector<uint32_t> data(genRate + prioRate);
  uint32_t idx = 0;
  cnt=0;
  while(cnt < genRate)
  {
    if ((idx < prioRate) && (prioNums[idx] <= cnt))
    {
      data[idx+cnt] = priov;
      idx++;
      data[idx+cnt] = genv;
      cnt++;
    }
    else
    {
      data[idx+cnt] = genv;
      cnt++;
    }
  }
  return data;
}

/* Method 3 */
vector<uint32_t> generateData3(uint32_t prioRate, uint32_t genRate)
{
  std::vector<uint32_t> result(prioRate + genRate, 5);
  for (uint32_t i = 0; i < prioRate; ++i) {
      result[i] = 7;
  }

  // Use Fisher-Yates shuffle to randomize the array
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(result.begin(), result.end(), g);

  return result;
}

// Debugging Functions
template<class T> void prettyPrint(std::vector<T>& v) { for (T x : v) {std::cout<<x<<std::endl;} }
template<class T, class V> void prettyPrint(std::pair<T,V> &p) {std::cout<<"first: "<<p.first<<"\nsecond: "<<p.second<<std::endl;}
template<class T, class V> void prettyPrint(std::map<T,V>& m) {for(auto x: m) {std::cout<<"key: "<<x.first<<"\nvalue: "<<x.second<<std::endl;}}


}

#endif