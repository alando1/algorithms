#include <boost/graph/dijkstra_shortest_paths_no_color_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stack>

using namespace boost;
using namespace std;

//typedefs for the Graph stuff, using string for label and int for weight with adjacency list.
typedef property<edge_name_t, string, property<edge_weight_t, int> > EdgeProperty;
typedef adjacency_list<listS, vecS, directedS, no_property, EdgeProperty> Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::edge_descriptor Edge;
typedef property_map<Graph, edge_name_t>::type NameMap;
typedef property_map<Graph, edge_weight_t>::type WeightMap;

int isValid(int i, int j, int rows, int cols);
void BuildGraph(Graph& g, vector<vector<string> >& vec, int rows, int cols, int& stop);
bool findJojo(Graph& g, stack<Edge>& path, int start, int stop, int vertices);
void WriteOutput(stack<Edge> path, NameMap nm, WeightMap wm);

int main()
{
	//opens file.
	ifstream infile;
	infile.open("input.txt");
	if(!infile.is_open()) 
	{
		cout << "\nFile input.txt could not be opened. Try again.\n" << endl;
		return -1;
	}

	//declare some variables along with graph
	int rows, cols, start, stop, starti, startj, vertices;
	vector<vector<string> > vec;
	Graph g;
	stack<Edge> path;
	
	//gets rows and columns, resizes vector, calculates vertices.
	infile >> rows >> cols;
	vertices = rows*cols;
	vec.resize(rows, vector<string>(cols));
	
	//input the starting row and col and calculates start vertex index number.
	infile >> starti >> startj;
	start = cols*(starti-1) + (startj-1);

	//get all the vertex directions.
	for(int i = 0; i < rows; ++i)
		for(int j = 0; j < cols; ++j)
			infile >> vec[i][j];

	//builds graph and finds shortest path if  exists.
	BuildGraph(g, vec, rows, cols, stop);
	if(!findJojo(g, path, start, stop, vertices)) 
	{
		cout << "\nThere is no path to Jojo." << endl;
		return -1;
	}

	//create weight and name maps derived from graph g, and call output function.
	WeightMap wm = get(edge_weight, g);
	NameMap nm = get(edge_name, g);
	WriteOutput(path, nm, wm);

	return 0;
}

//checks to make sure vertex is valid, then calculates its vertex number.
//returns -1 if it doesnt exist.
int isValid(int i, int j, int rows, int cols) {

	if(i >=0 && i < rows && j >=0 && j < cols)
		return i*cols+j;

	return -1;
}

//builds graph by adding all the edges that exist to the Graph.
void BuildGraph(Graph& g, vector<vector<string> >& vec, int rows, int cols, int& stop) {

	int vertex, temp, tempi[2], tempj[2];
	for(int i = 0; i < rows; ++i) {

		for(int j = 0; j < cols; ++j) {

			//skip all marked X
			if(vec[i][j] == "X")
				continue;

			//gets jojo's indexed vertex number. No outbound edges on J, only inbound.
			else if(vec[i][j] == "J") {
				stop = i*cols+j;
				continue;
			}

			//based on direction calculates neighbor index i and j for both neighbors 3 and 4.
			else if(vec[i][j] == "N") {
				tempi[0] = i-3, tempj[0] = j;
				tempi[1] = i-4, tempj[1] = j;
			}

			else if(vec[i][j] == "NE") {
				tempi[0] = i-3, tempj[0] = j+3;
				tempi[1] = i-4, tempj[1] = j+4;
			}

			else if(vec[i][j] == "E") {
				tempi[0] = i, tempj[0] = j+3;
				tempi[1] = i, tempj[1] = j+4;
			}

			else if(vec[i][j] == "SE") {
				tempi[0] = i+3, tempj[0] = j+3;
				tempi[1] = i+4, tempj[1] = j+4;
			}

			else if(vec[i][j] == "S") {
				tempi[0] = i+3, tempj[0] = j;
				tempi[1] = i+4, tempj[1] = j;
			}

			else if(vec[i][j] == "SW") {
				tempi[0] = i+3, tempj[0] = j-3;
				tempi[1] = i+4, tempj[1] = j-4;
			}

			else if(vec[i][j] == "W") {
				tempi[0] = i, tempj[0] = j-3;
				tempi[1] = i, tempj[1] = j-4;
			}

			else if(vec[i][j] == "NW") {
				tempi[0] = i-3, tempj[0] = j-3;
				tempi[1] = i-4, tempj[1] = j-4;
			}

			//calculate current vertex index.
			vertex = i*cols+j;

			//makes sure neighboring vertex exists and isnt an X, adds an edge with weight, direction, and label if it exists.
			temp = isValid(tempi[0], tempj[0], rows, cols);
			if(temp != -1 && vec[tempi[0]][tempj[0]] != "X")
				add_edge(vertex, temp, EdgeProperty(vec[i][j], 3), g);

			temp = isValid(tempi[1], tempj[1], rows, cols);
			if(temp != -1 && vec[tempi[1]][tempj[1]] != "X")
				add_edge(vertex, temp, EdgeProperty(vec[i][j], 4), g);
		}
	}
}

//create a predecessor map using dijkstra's algorithm with direction and weights.
//use stack and the predecessor map to get the path from tarzan's start to jojo/finish.
bool findJojo(Graph& g, stack<Edge>& path, int start, int stop, int vertices) {

	//start vertex, stop vertex, predecessor and name maps.
	Vertex vstart = vertex(start, g);
	Vertex v = vertex(stop, g);
	vector<Vertex> pred(vertices);
	vector<int> dist(vertices);

	//runs dijkstras and creates a pred map.
	dijkstra_shortest_paths_no_color_map(g, vstart, distance_map(&dist[0]).predecessor_map(&pred[0]));

	//pushes all edges from pred onto path, reordering the list from start to finish for popping.
	for(Vertex u = pred[v]; u != v; v = u, u = pred[v])
		path.push(edge(u, v, g).first);
	
	//return true if a path from tarzan to Jojo exists,return false if no path.
	if(v == vstart)
		return true;
	else
		return false;
}

//function outputs information to output file
void WriteOutput(stack<Edge> path, NameMap nm, WeightMap wm) {

	//opens/creates new output.txt file to write to.
	ofstream outfile;
	outfile.open("output.txt");

	//prints to output.txt edges for the shortest path to Jojo.
	while(!path.empty()) {

		//cout << nm[path.top()] << "-" << wm[path.top()] << " "; //debug

		//saves edge label and edge weigth to the output.txt and pops the stack.
		outfile << nm[path.top()] << "-" << wm[path.top()] << " ";
		path.pop();
	}
	//cout << "\n"; //debug

	//adds new line and closes the file.
	outfile << '\n';
	outfile.close();
}
