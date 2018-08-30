#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

struct _student {
	string name;
	int score;

	// sort by score low to high 
	bool operator<(const _student &other_stu) {
		return score!= other_stu.score ? score < other_stu.score : name < other_stu.name;
	}

	friend ostream& operator<<(ostream &os, const _student &stu) {
		os << "student: " << stu.name << ", " << stu.score << endl;
		return os;
	}
};
typedef struct _student stu;

template<typename T>
void selectionSort(T arr[], int n) {
	int minIndex;
	for(int i = 0; i < n; ++i) {
		minIndex = i;
		for(int j = minIndex; j < n; ++j) {
			if(arr[j] < arr[minIndex]) {
				minIndex = j;
			}
		}
		swap(arr[i], arr[minIndex]);
	}
}

int main() {

	

	int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	selectionSort(a, 10);
	for(int i = 0; i < 10; ++i) {
		cout << a[i] ;
	}
	cout << endl;
	
	char s[4] = {'a', 'B', 'C', 'z'};
	selectionSort(s, 4);
	for(int i = 0; i < 4; ++i) {
		cout << s[i];
	}
	cout << endl;
	
	stu d[4] = { {"b", 95}, {"c", 93}, {"tt", 92}, {"Hz", 91}};
	selectionSort(d, 4);
	for(int i = 0; i < 4; ++i) {
		cout << d[i];
	}
	cout << endl;
	
	return 0;
}
