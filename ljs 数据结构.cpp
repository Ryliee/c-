#include<iostream>
#include<stdio.h>
#include<string>
#include<string.h>
#include<map>
#include <stdlib.h>
#include<cmath>
#include <winsock.h>
#include<ctime>
#include <cstring>
#include<time.h>


#define Step 5
#define merNum 1000
#define PI 3.1415926
double R = 6378137.0;

using namespace std;
map<string, string> base32;



struct geolocation {
	double longitude;
	double latitude;
	string name;
	string loc;
};

struct location {
	double longitude;
	double latitude;
	string name;
};

struct res {
	int distance;
	string name;
	res* next;
	res* prior;
};

struct Node {
	Node* member;
	Node* next;
	Node* prior;
	double longitude;
	double latitude;
	string name;
	string loc;
};

location merchantLoc[merNum];
geolocation geo[merNum];


class database {
public:
	void init(string type);
};
class Geohash :public database {
public:
	void init();
	void calculate(string usersLoc, double lat, double lon);
	void print();
private:
	Node* first;
	res* List;
};
class search :public database {
public:
	void insert(double longitude, double latitude);
	void print();
private:
	res* first;
};

string cut(string s, int length) {
	string temp;
	for (int i = 0; i < length; i++)
	{
		temp += s[i];
	}
	return temp;
}

void Geohash::calculate(string usersLoc, double userLat, double userLog) {
	int length = usersLoc.size();
	List = new res;
	List->distance = 0;
	List->next = NULL;
	for (int i = length; i >= 0; i--) {
		string searchLoc = cut(usersLoc, i);
		Node* s = first;
		while (s->next != NULL) {
			s = s->next;
			Node*position = s;
			if (searchLoc == cut(s->loc, i)||cut(s->loc,i)=="") {
				s = s->member;
				int j = 0;
				while (1) {
					res* p = List;
					res* r = new res;
					double merLog = (s->longitude * PI / 180.0);
					double merLat = (s->latitude * PI / 180.0);
					double log = userLog - merLog;
					double lat = userLat - merLat;
					double distance = 2 * asin(sqrt(pow(sin(lat / 2), 2) + cos(userLat) * cos(merLat) * pow(sin(log / 2), 2)));
					distance *= R;
					distance = round(distance * 10000) / 10000;
					r->next = NULL;
					r->distance = distance;
					r->name = s->name;
					while (1) {
						if (r->distance > p->distance&& p->next != NULL) {
							p = p->next;
							continue;
						}
						else if (r->distance > p->distance) {
							p->next = r;
							r->prior = p;
							break;
						}
						else {
							r->prior = p->prior;
							p->prior->next = r;
							p->prior = r;
							r->next = p;
							break;
						}
					}
					if (s->next != NULL) {
						s = s->next;
					}
					else { 
						break; 
					}
				}
				s = position;
				if (s->next != NULL) {
					s->next->prior = s->prior;
					s->prior->next = s->next;
				}
				else {
					s->prior->next = NULL;
				}
			}
		}
	}
}

void Geohash::init() {
	first = new Node;
	first->loc = "";
	first->next = NULL;
	for (int i = 0; i < merNum; i++) {
		Node* r = first;
		while (1) {
			if (r->loc != geo[i].loc && r->next != NULL) {
				r = r->next;
			}
			else if (r->loc != geo[i].loc) {
				Node* s = new Node;
				s->next = NULL;
				s->loc = geo[i].loc;
				Node* p = new Node;
				p->latitude = geo[i].latitude;
				p->longitude = geo[i].longitude;
				p->name = geo[i].name;
				p->next = NULL;
				s->member = p;
				s->prior = r;
				r->next = s;
				break;
			}
			else {
				Node* s = new Node;
				s->latitude = geo[i].latitude;
				s->longitude = geo[i].longitude;
				s->name = geo[i].name;
				s->next = r->member;
				r->member = s;
				break;
			}
		}
	}
}

void Geohash::print() {
	cout << "从近到远排序为：" << endl;
	res* s = List;
	for (int i = 0; i < merNum; i++) {
		s = s->next;
		cout << "name: " << s->name << " " << "distance: " << s->distance  << "米" << endl;
	}
}

void search::print() {
	cout << "从近到远排序为：" << endl;
	res* s = first;
	for (int i = 0; i < merNum; i++) {
		s = s->next;
		cout << "name: " << s->name << " " << "distance: " << s->distance  << "米" << endl;
	}
}

void search::insert(double longitude, double latitude) {
	first = new res;
	first->distance = 0;
	first->next = NULL;
	res* p = new res;
	p->next = NULL;
	p = first->next;
	double userLog = (longitude * PI / 180.0);
	double userLat = (latitude * PI / 180.0);
	for (int i = 0; i < merNum; i++) {
		res* r = first;
		res* s = new res;
		s->next = NULL;
		double merLog = (merchantLoc[i].longitude * PI / 180.0);
		double merLat = (merchantLoc[i].latitude * PI / 180.0);
		double log = userLog - merLog;
		double lat = userLat - merLat;
		double distance = 2 * asin(sqrt(pow(sin(lat / 2), 2) + cos(userLat) * cos(merLat) * pow(sin(log / 2), 2)));
		distance *= R;
		distance = round(distance * 10000) / 10000;
		s->distance = distance;
		s->name = merchantLoc[i].name;
		while (1) {
			if (s->distance > r->distance&& r->next != NULL) {
				r = r->next;
				continue;
			}
			else if (s->distance > r->distance) {
				r->next = s;
				s->prior = r;
				break;
			}
			else {
				s->prior = r->prior;
				r->prior->next = s;
				r->prior = s;
				s->next = r;
				break;
			}
		}
	}
}

string GeohashAdd(string longitude, string latitude) {
	string ADD_BIN;
	for (int i = 0; i < longitude.size(); i++) {
		ADD_BIN += longitude[i];
		ADD_BIN += latitude[i];
	}
	return ADD_BIN;
}

string GeohashCoordinateBin(double loc, double min, double max, int step, int max_step) {
	if (step > max_step)//规定精度 
	{
		return "";
	}
	double mid = (min + max) * 1.0 / 2;
	if (loc >= min && loc <= mid) {
		return GeohashCoordinateBin(loc, min, mid, step + 1, max_step) + "0";
	}
	if (loc >= mid && loc <= max) {
		return GeohashCoordinateBin(loc, mid, max, step + 1, max_step) + "1";
	}
}

string GeohashSeparate(string decode, string type) {
	string s;
	if (type == "longtitude") {
		for (int i = 0; i < decode.size() * 2; i += 2) {
			s += decode[i];
		}
	}
	else {
		for (int i = 0; i < decode.size() * 2; i += 2) {
			s += decode[i + 1];
		}
	}
	return s;
}

string GeohashBase32(string ADD_BIN) {
	string temp;
	string base;
	for (int i = 0; i < ADD_BIN.size(); i++) {
		temp += ADD_BIN[i];
		if ((i + 1) % 5 == 0) {
			base += base32[temp];
			temp = "";
		}//这步是每五位进行一次调用
	}
	return base;
}

string decode(string code) {
	string s;
	for (int i = 0; i < code.size(); i++)
	{
		for (std::map<string, string>::iterator iter = base32.begin(); iter != base32.end(); iter++) {
			string temp(1, code[i]);
			if (iter->second == temp) {
				s += iter->first;
				break;
			}
		}
	}
	return s;
}



void database::init(string type) {
	srand((unsigned)time(NULL));
	if (type == "geohash") {
		for (int i = 0; i < merNum; i++) {
			double lon = rand() % 360 - 180+ double(rand() % 1000) / 1000 + double(rand() % 1000) / 1000000;
			double lat = rand() % 180 - 90+ double(rand() % 1000) / 1000 + double(rand() % 1000) / 1000000;
			string longitude = GeohashCoordinateBin(lon, -180, 180, 0, Step);
			string latitude = GeohashCoordinateBin(lat, -90, 90, 0, Step);
			string ADD_BIN = GeohashAdd(longitude, latitude);
			string base = GeohashBase32(ADD_BIN);
			geo[i].name = "商户" + to_string(i);
			geo[i].loc = base;
			geo[i].longitude = lon;
			geo[i].latitude = lat;
		}
	}
	else {
		for (int i = 0; i < merNum; i++) {
			double lon = rand() % 360 - 180 + double(rand() % 1000) / 1000 + double(rand() % 1000) / 1000000;
			double lat = rand() % 180 - 90 + double(rand() % 1000) / 1000 + double(rand() % 1000) / 1000000;
			merchantLoc[i].longitude = lon;
			merchantLoc[i].latitude = lat;
			merchantLoc[i].name = "商户" + to_string(i);
		}
	}
}

void init() {
	base32["00000"] = "0";
	base32["00001"] = "1";
	base32["00010"] = "2";
	base32["00011"] = "3";
	base32["00100"] = "4";
	base32["00101"] = "5";
	base32["00110"] = "6";
	base32["00111"] = "7";
	base32["01000"] = "8";
	base32["01001"] = "9";
	base32["01010"] = "b";//10
	base32["01011"] = "c";//11
	base32["01100"] = "d";//12
	base32["01101"] = "e";//13
	base32["01110"] = "f";//14
	base32["01111"] = "g";//15
	base32["10000"] = "h";//16
	base32["10001"] = "j";//17
	base32["10010"] = "k";//18
	base32["10011"] = "m";//19
	base32["10100"] = "n";//20
	base32["10101"] = "p";//21
	base32["10110"] = "q";//22
	base32["10111"] = "r";//23
	base32["11000"] = "s";//24
	base32["11001"] = "t";//25
	base32["11010"] = "u";//26
	base32["11011"] = "v";//27
	base32["11100"] = "w";//28
	base32["11101"] = "x";//29
	base32["11110"] = "y";//30
	base32["11111"] = "z";//31
}
void main() {
	init();
	database db;
	search src;
	Geohash geo;
	db.init("geohash");
	geo.init();
	double log, lat;
	cout << "please put in longtitude:" << endl;
	cin >> log;
	cout << "please put in latitude:" << endl;
	cin >> lat;
	string longitude = GeohashCoordinateBin(log, -180, 180, 0, Step);
	string latitude = GeohashCoordinateBin(lat, -90, 90, 0, Step);
	string ADD_BIN = GeohashAdd(longitude, latitude);
	string base = GeohashBase32(ADD_BIN);
	clock_t start = clock();
	geo.calculate(base,log,lat);
	geo.print();
	clock_t ends = clock();
	cout << "运行时间:" << (double)(ends - start) / CLOCKS_PER_SEC << "s" << endl;
	/*src.insert(110.23, -73.11);
	src.print();*/
	system("pause");
}
