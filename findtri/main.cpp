#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define D 16
#define HD 8
#define sqrd1 256
#define N 21010000
#define W 1000000

int x[N], y[N];

inline long long sqr(long long x) {
	return x*x;
}

inline long long sqrdist(int l, int r) {
	long long sqrd = sqr(x[l] - x[r]) + sqr(y[l] - y[r]);
	return sqrd;
}

inline double dist(int l,int r) {
	return sqrt(sqrdist(l,r));
}

inline double getarea(double a,double b,double c) {
	double p = 0.5*(a + b + c);
	return sqrt(p*(p - a)*(p - b)*(p - c));
}

inline void printpts2(int i, int j) {
	cout << x[i] << " " << y[i] << " " << x[j] << " " << y[j] << endl;
}


int main() {
	int n = 0;
	for (int fi = 9;fi >=0;fi--) {
		string filename("Yourpath\\f"+to_string(fi)+".txt");// change here to your input path. You can also use a single file instead.
		cout << "Reading " << filename;
		fstream infile(filename);
		while (!infile.eof()) {
			infile >> x[n] >> y[n];
			n++;
		}
		infile.close();
		n--;
		cout << " : " << n << endl;
	}
	for (int fi = 0;fi <=9;fi++) {
		string filename("Yourpath\\z" + to_string(fi) + ".txt");
		cout << "Reading " << filename;
		fstream infile(filename);
		while (!infile.eof()) {
			infile >> x[n] >> y[n];
			n++;
		}
		infile.close();
		n--;
		cout << " : " << n << endl;
	}
	
	long long minsqrd = 0x7fffffffffffffff;
	int pminsqrdi = -1;
	int pminsqrdj = -1;
	ofstream outf("out.txt",'w');
	for (int i = 0;i < n;i++) {
		// for symmetry, we only consider p2 and p3 > p1
		if (i % 1000000 == 0)
			cout << i << endl;
		int r = i + 1;
		while (r < n) {
			if (x[r] - x[i] > D)break;
			r++;
		}
		for (int j = i + 1;j < r - 1;j++) {
			long long sqrd = sqrdist(i, j);
			if (sqrd > sqrd1)continue;
			if (sqrd < minsqrd) {
				minsqrd = sqrd;
				pminsqrdi = i;
				pminsqrdj = j;
			}
			double a = sqrt(sqrd);
			for (int k = j + 1;k < r;k++) {
				
				long long sqrd = sqrdist(i, k);
				if (sqrd > sqrd1)continue;
				double b = sqrt(sqrd);

				// test if they are at the same line
				if ((y[k] - y[i])*(x[j] - x[i]) == (x[k] - x[i])*(y[j] - y[i])) {
					// if they are at same line, test if the longest distance < D
					if (sqrd <= sqrd1) {
						outf << x[i] << " " << y[i] << " " << x[j] << " " << y[j] << " " << x[k] << " " << y[k] << endl;
					}
					continue;
				}

				sqrd = sqrdist(j, k);
				if (sqrd > sqrd1)continue;
				double c = sqrt(sqrd);
				double s = getarea(a, b, c);
				double radius = a*b*c / (4 * s);
				if (radius <= HD)
					outf<< x[i] << " " << y[i] << " " << x[j] << " " << y[j] << " " << x[k] << " " << y[k] << endl;
			}

		}
	}
	cout << minsqrd << " " << x[pminsqrdi] << " " << y[pminsqrdi] << " " << x[pminsqrdj] << " " << y[pminsqrdj] << endl;
	outf.close();
	system("pause");
	return 0;
}
