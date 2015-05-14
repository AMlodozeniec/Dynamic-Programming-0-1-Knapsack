#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <ctime>

using namespace std;

//Structs
typedef struct{
	string name;
	int weight;
	int marketPrice;
	int profit;
} Card;

typedef struct{
	vector<Card> gCards;
	vector<Card> bestSet;
	vector<Card> all;
	int currentGertrudeSize;
	int capacity;
	int weightSoFar;
	int maxProfit;
} Knapsack;

//Globals
Knapsack knapsack;
clock_t start, end;
double time_spent;

/* Finds the max integer between two integers */
int max(int a, int b){
	if(a > b){
		return a;
	}
	else{
		return b;
	}
}

/* Prints the usage string */
void usageString(){
	cout << "Usage: ./program4 -m <market-price-file> -p <price-list-file> -o <output-file-name> [3]" << endl;
}

/* Prints contents of vector */
void printVectorContents(vector<Card> vec){
	cout << "-VECTOR-" << endl;
	for(int i = 1; i < vec.size(); ++i){
		cout << "Name: " << vec[i].name << " Weight: " << vec[i].weight << " Price: " << vec[i].marketPrice;
		cout << " Profit: " << vec[i].profit << endl;
		cout << "i: " << i << " vec.size() = " << vec.size() << endl;
	}
}

/* Allocates vector with values from marketfile */
void readMarketPrices(vector<Card> &cards, ifstream &marketFile){
	//cout << "-READING MARKET PRICE-" << endl;
	string size, line;
	int mSize;
	getline(marketFile, size);
	mSize = atoi(size.c_str());
	if(mSize != 0){
		cards.resize(mSize+1); //I want index to start at 1
	}
	for(int i = 1; i <= mSize; i++){
		getline(marketFile, line);
		stringstream ss(line);
	//	cout << line << endl;
		int price;
		string name;
		if(ss >> name >> price){
			cards[i].name = name;
			cards[i].marketPrice = price;
			cards[i].profit = -1;
		}
	}
	//printVectorContents(cards);
}

/* Checks if vector has same card as Gertrudes list and if so, sets profit */
void readGertrudeCards(vector<Card> &cards, ifstream &priceFile){
	//cout << "-READING GERTRUDE" << endl;
	string line, name;
	int price = 0;
	for(int i = 1; i < cards.size(); i++){
		getline(priceFile, line);	
		stringstream ss(line);
	//	cout << line << endl;
		if(ss >> name >> price){
			cards[i].name = name;
			cards[i].weight = price;
		}
	}
	//printVectorContents(cards);
}

/* Computes profit */
void setValues(vector<Card> &cards, vector<Card> &gCards){
	//cout << "-COMPUTING PROFIT-" << endl;
	for(int i = 1; i < cards.size(); ++i){
		for(int j = 1; j < cards.size(); ++j){
			if(gCards[j].name == cards[i].name){
				gCards[j].profit = cards[i].marketPrice - gCards[j].weight;
			}
		}
	}
	//printVectorContents(gCards);
}


/* Creates table from bottom up, returns maxProfit */
vector<Card> maxProfitDynamic(Knapsack knapsack, ofstream &output){
	//cout << "-DYNAMIC PROGRAMMING-" << endl;
	start = clock();
	int size = knapsack.currentGertrudeSize;
	int capacity = knapsack.capacity;
	vector<Card> vec = knapsack.gCards;
	
	int P[size + 1][capacity + 1];
	for(int c = 0; c <= capacity; c++){
		P[0][c] = 0;
	}
	for(int i = 0; i <= size; i++){
		P[i][0] = 0;
	}	
	for(int i = 1; i <= size; i++){
		for(int c = 1; c <= capacity; c++){
			if(vec[i].weight <= c && (P[i-1][c-vec[i].weight] + vec[i].profit > P[i-1][c])){
				P[i][c] = max(P[i-1][c-vec[i].weight] + vec[i].profit, P[i-1][c]);
			}
			else{
				P[i][c] = P[i-1][c];
			}
		}
	}
	end = clock();
	time_spent = (double) (end - start);
	/* Printing the table */
	/*cout << "-TABLE-" << endl;
	for(int i = 0; i <= size; i++){
		for(int j = 0; j <= capacity; j++){
			cout << P[i][j] << " ";
		}
		cout << endl;
	}
	*/
	//cout << "-PRINTING OPTIMAL SET-" << endl;
	int i = size;
	int c = capacity;
	//printVectorContents(vec);
	knapsack.bestSet.resize(1);
	while(i > 0 && c > 0){
	//	cout << "i: " << i << " c: " << c << endl;
		if(P[i][c] != P[i-1][c]){
			knapsack.bestSet.push_back(vec[i]);
			c = c - vec[i].weight;
			i--;
		}
		else{
			i--;
		}
	}
	for(int i = 1; i < knapsack.bestSet.size(); ++i){
		knapsack.maxProfit += knapsack.bestSet[i].profit;
	}
	output << "Dynamic Programming: " << size << " " << knapsack.maxProfit << " " << time_spent << endl;
	//printVectorContents(knapsack.bestSet);
	return knapsack.bestSet;
}

int main(int argc, char *argv[]){
	//Parse Command Line Arguments
	const char *marketfile;
	const char *pricefile;
	const char *output;
	int outputType;
	
	if(argc < 8){
		cout << "Not enough parameters" << endl;
		usageString();
		return EXIT_FAILURE;
	}
	else{
		for(int i = 1; i < argc; i += 2){
			if(i + 1 != argc){
				if(string(argv[i]) == "-m"){
					marketfile = argv[i+1];
				}
				else if(string(argv[i]) == "-p"){
					pricefile = argv[i+1];
				}
				else if(string(argv[i]) == "-o"){
					output = argv[i+1];
					outputType = atoi(argv[i+2]);
					if(outputType != 3){
						cout << "OutputType must equal 3" << endl;
						usageString();
						return EXIT_FAILURE;
					}
				}
				else{
					cout << "Invalid arguments" << endl;
					return EXIT_FAILURE;
				}
			}
		}
	}
	
	ifstream marketFile(marketfile);
	if(!marketFile.is_open()){
		cerr << "Error: Market price file doesn't exist" << endl;
		return EXIT_FAILURE;
	}
	
	ifstream priceFile(pricefile);
	if(!priceFile.is_open()){
		cerr << "Error: Price file doesn't exist" << endl;
		return EXIT_FAILURE;
	}
		
	ofstream outFile(output);
	if(!outFile.is_open()){
		cerr << "Error: Outputfile doesn't exist" << endl;
		return EXIT_FAILURE;
	}
	
	//Setting market prices
	readMarketPrices(knapsack.all, marketFile);
	//cout << "FINISHED" << endl;
	//Reading the file
	string line, name;
	int capacity = 0;
	int size = 0;
	vector<Card> bestSet;
	while(getline(priceFile, line)){
		stringstream ss(line);
	//	cout << "-READING INPUT-" << endl;
	//	cout << line << endl;
		if(ss >> size >> capacity){
			knapsack.capacity = capacity;
			knapsack.currentGertrudeSize = size;
			knapsack.gCards.resize(knapsack.currentGertrudeSize + 1);
			readGertrudeCards(knapsack.gCards, priceFile);
			setValues(knapsack.all, knapsack.gCards);
			bestSet = maxProfitDynamic(knapsack, outFile);
			knapsack.bestSet = bestSet;
	//		printVectorContents(bestSet);
			
		}
	}
		
		
	return 1;
}
