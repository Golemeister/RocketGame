#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct Graph{
	
	int numberOfNodes;
	int** map;
}Graph;

typedef struct Mine {
	float x, y, r;
	bool isDetonated;
	int numberOfDetonations;
}Mine;

typedef struct MineField {
	Mine* mines;
	int numberOfMines;
}MineField;

Graph makeGraph(int numberOfNodes) {
	Graph newGraph;
	
	newGraph.map = (int**)malloc(numberOfNodes * sizeof(int*));
	for (int i = 0; i < numberOfNodes; i++) {
		newGraph.map[i] = (int*)calloc(numberOfNodes,sizeof(int));
	}
	for (int i = 0; i < numberOfNodes; i++) {
		for (int j = 0; j < numberOfNodes; j++) {
		newGraph.map[i][j] = 0;
		}
	}

	newGraph.numberOfNodes = numberOfNodes;

	return newGraph;
}

void addNewNode(Graph* graph) {

	graph->numberOfNodes++;
	graph->map = (int**)realloc(graph->map, graph->numberOfNodes * sizeof(int*));
	graph->map[graph->numberOfNodes - 1] = (int**)calloc(graph->numberOfNodes, sizeof(int*));

	for (int i = 0; i < graph->numberOfNodes - 1; i++) {
		
 			graph->map[i] = (int*)realloc(graph->map[i], graph->numberOfNodes * sizeof(int));
			graph->map[i][graph->numberOfNodes-1] = 0;
			graph->map[graph->numberOfNodes-1][i] = 0;
		
	}

}

void addNewLink(Graph* graph) {
	
	int firstNode, secondNode;
	printf("Unesi vezu A->B\n");
	scanf("%d%d", &firstNode, &secondNode);
	if (firstNode < 0 || firstNode > graph->numberOfNodes - 1 || secondNode < 0 || secondNode > graph->numberOfNodes - 1) {
		printf("Van opsega!\n");
		return;
	}
	else
	{
		graph->map[firstNode][secondNode] = 1;
	}
}

void removeNode(Graph* graph) {

	int nodeForDelete;
	printf("Unesi broj cvora za brisanje\n");
	scanf("%d", &nodeForDelete);
	if (nodeForDelete < 0 || nodeForDelete>graph->numberOfNodes - 1) {
		printf("Cvor je van opsega!\n");
		return;
	}
	else
	{
		for (int i = 0; i < graph->numberOfNodes; i++) {
			if (i != nodeForDelete) {
				graph->map[i][nodeForDelete] = 0;
			}
		}

		for (int i = nodeForDelete; i < graph->numberOfNodes-1; i++) {
			for (int j = 0; j < graph->numberOfNodes; j++) {
				graph->map[i][j] = graph->map[i + 1][j];
				graph->map[j][i] = graph->map[j][i + 1];
				
			}
			graph->map[i][i] = 0;
		}
		graph->numberOfNodes--;
		graph->map = (int**)realloc(graph->map, graph->numberOfNodes * sizeof(int*));


		for (int i = 0; i < graph->numberOfNodes; i++) {

			graph->map[i] = (int*)realloc(graph->map[i], graph->numberOfNodes * sizeof(int));
		}
	}
}

void deleteGraph(Graph* graph) {
	for (int i = 0; i < graph->numberOfNodes; i++) {
		free(graph->map[i]);
	}
	free(graph->map);
	graph->numberOfNodes = 0;
}

void printGraph(Graph graph) {
	for (int i = 0; i < graph.numberOfNodes; i++) {
		printf("%d:[ ",i);
		for (int j = 0; j < graph.numberOfNodes; j++) {
			printf("%d ",graph.map[i][j]);
		}
		printf("]\n");
	}
}

MineField makeMineField() {

	printf("Unesi naziv fajla za citanje mina\n");
	char fileName[20];
	scanf("%s", &fileName);
	FILE* chosenFile = fopen(fileName, "r");
	MineField newMineField;
	fscanf(chosenFile, "%d", &newMineField.numberOfMines);
	newMineField.mines = (Mine*)calloc(newMineField.numberOfMines, sizeof(Mine));
	float x, y, r;
	for (int i = 0; i < newMineField.numberOfMines; i++) {
		fscanf(chosenFile, "%f %f %f", &x, &y, &r);
		Mine newMine;
		newMine.x = x;
		newMine.y = y;
		newMine.r = r;
		newMine.isDetonated = false;
		newMine.numberOfDetonations = 0;
		newMineField.mines[i] = newMine;
	}

	return newMineField;
}

bool isInRange(Mine* mine1, Mine* mine2) {
	float dx, dy,dd;
	dx = (mine1->x - mine2->x);
	dy = (mine1->y - mine2->y);
	dd = sqrt(dx * dx + dy * dy);
	if (mine1->r >= dd) {
		return true;
	}
	else
	{
		return false;
	}
}

int detonateCurrentMine(MineField* mineField, int mineNumber) {
	Mine* activatedMine = &mineField->mines[mineNumber];
	int numberOfDetonations = 0;
	if (activatedMine->isDetonated) {
		return 0;
	}
	else {
		numberOfDetonations++;
		activatedMine->isDetonated = true;

		printf("Detonirana mina broj %d\n", mineNumber);

		for (int i = 0; i < mineField->numberOfMines; i++) {
			Mine* currentMine = &mineField->mines[i];
			if (i != mineNumber && !currentMine->isDetonated) {
				if (isInRange(activatedMine, currentMine)) {
					numberOfDetonations += detonateCurrentMine(mineField, i);
				}
			}
		}
		return numberOfDetonations;
	}
}

Mine makeRocket() {
	Mine newRocket;
	float x, y, r;

	printf("Unesi x y i r rakete\n");
	scanf("%f%f%f", &x, &y, &r);
	newRocket.x = x;
	newRocket.y = y;
	newRocket.r = r;
	newRocket.isDetonated = false;
	newRocket.numberOfDetonations = 0;

	return newRocket;
}

int rocketDetonation(MineField* mineField,Mine* rocket) {
	int numberOfDetonations = 0;
	for (int i = 0; i < mineField->numberOfMines; i++) {
		Mine* currentMine = &mineField->mines[i];
		if (isInRange(rocket, currentMine)) {
			numberOfDetonations += detonateCurrentMine(mineField, i);
		}
	}
	return numberOfDetonations;
}

int findMostEfficentMine(MineField* mineField) {
	int mostEfficentMine = 0;
	int maxDetonations = 0;

	for (int i = 0; i < mineField->numberOfMines; i++) {
		for (int j = 0; j < mineField->numberOfMines; j++) {
			mineField->mines[j].isDetonated = false;
		}
		int currentDetonations = detonateCurrentMine(mineField, i);
		if (currentDetonations > maxDetonations) {
			maxDetonations = currentDetonations;
			mostEfficentMine = i;
		}
		
	}
	printf("Najefikasnija mina je (%f)(%f) sa radijusom (%f) i brojem detonacija %d\n", mineField->mines[mostEfficentMine].x, mineField->mines[mostEfficentMine].y, mineField->mines[mostEfficentMine].r,maxDetonations);
	return mostEfficentMine;
}

float findAreaOfDetonations(MineField* mineField, int mineNumber ) {
	Mine* activatedMine = &mineField->mines[mineNumber];
	float areaOfDetonation = 0;
	if (activatedMine->isDetonated) {
		return 0.0;
	}
	else {
		
		activatedMine->isDetonated = true;
		for (int i = 0; i < mineField->numberOfMines; i++) {
			Mine* currentMine = &mineField->mines[i];
			if (i != mineNumber && !currentMine->isDetonated) {
				if (isInRange(activatedMine, currentMine)) {
					areaOfDetonation = calculateDetonation(activatedMine, currentMine,areaOfDetonation);
					if (areaOfDetonation < 0) areaOfDetonation = 0;
					areaOfDetonation += findAreaOfDetonations(mineField, i);
				}
			}
		}
		return areaOfDetonation;
	}
}

float calculateDetonation(Mine* mine1, Mine* mine2,float areaOfDetonation) {
	float distance = sqrt(pow((mine1->x - mine2->x), 2) + pow((mine1->y - mine2->y), 2));
	float area;
	if (distance <= mine1->r - mine2->r) {
		area = areaOfDetonation;
	}
	else {
		float angle1 = 2 * acos((pow(mine1->r, 2) - pow(mine2->r, 2) + pow(distance, 2)) / (2 * mine1->r * distance));
		float angle2 = 2 * acos((pow(mine2->r, 2) - pow(mine1->r, 2) + pow(distance, 2)) / (2 * mine2->r * distance));
		float area1 = pow(mine1->r, 2) * angle1 * 0.5;
		float area2 = pow(mine2->r, 2) * angle2 * 0.5;
		float triangle = distance * mine1->r * mine2->r * 0.5;
		float intersectionArea = area1 + area2 - triangle;
		area =  3.14 * pow(mine2->r, 2) - intersectionArea;
	}
	return area;
}

int main() {
	int numberOfNodes;
	Graph graph;
	MineField mineField;
	Mine rocket;
	int numberOfDetonations;
	int mostEfficentMine;
	int choice;
	float areaOfDetonation;
	while (true) {
		printf("[1]Formiraj graf\n[2]Dodaj cvor\n[3]Dodaj relaciju\n[4]Obrisi cvor\n[5]Obrisi graf\n[6]Ispis grafa\n\n[7]Formiraj minsko polje\n[8]Pronadji efikasnost odredjene mine\n[9]Pronadji najefikasniju minu\n[10]Lansiraj raketu\n[11]Povrsina explozije najefikasnije mine\n\n[0]Exit\n");
		scanf("%d", &choice);
		switch (choice) {
		case 0: exit(1);
			break;
		case 1: printf("Unesi broj cvorova grafa\n");
				scanf("%d", &numberOfNodes);
				graph = makeGraph(numberOfNodes);
			break;
		case 2: addNewNode(&graph);
			break;
		case 3: addNewLink(&graph);
			break;
		case 4: removeNode(&graph);
			break;
		case 5: deleteGraph(&graph);
			break;
		case 6: printGraph(graph);
			break;
		case 7: mineField = makeMineField();
		
			break;
		case 8: printf("Unesi broj mine koju zelis da detoniras\n");
				scanf("%d", &choice);
				numberOfDetonations = detonateCurrentMine(&mineField, choice);
				for (int j = 0; j < mineField.numberOfMines; j++) {
					mineField.mines[j].isDetonated = false;
				}
			break;
		case 9: mostEfficentMine = findMostEfficentMine(&mineField);
			for (int j = 0; j < mineField.numberOfMines; j++) {
				mineField.mines[j].isDetonated = false;
			}
			break;
		case 10:
				rocket = makeRocket();
				numberOfDetonations = rocketDetonation(&mineField, &rocket);
				for (int j = 0; j < mineField.numberOfMines; j++) {
					mineField.mines[j].isDetonated = false;
				}
			break;
		case 11:
				areaOfDetonation = 3.14*pow(mineField.mines[mostEfficentMine].r,2);
				areaOfDetonation += findAreaOfDetonations(&mineField,mostEfficentMine);
				printf("Povrsina detonacije je :%f\n", areaOfDetonation);
				for (int j = 0; j < mineField.numberOfMines; j++) {
					mineField.mines[j].isDetonated = false;
				}
			break;
		}
	}
	return 0;
}