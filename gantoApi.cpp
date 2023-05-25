#include "tableList.cpp"
#include <iostream>
using namespace std;

class api{
	private:
	double nullDouble = 10011100101010.101001100010011;		//"NULL" in binary is 01001110010101010100110001001100
	tableList tables;
	vector<int> getAcceptedEntries(table workingTable, vector<tuple<string, int, variant<string, double>>> conditions);
	bool compare(variant<string, double> left, int op, variant<string, double> right);	
	bool keyUsed(table* workingTable, int columnPos, variant<string, double> data);
	bool isRequired(table* workingTable, string column);
	vector<vector<int>> pairEntries(vector<vector<int>> base, string baseTableName, int tablePos, vector<int> acceptedEntries, string baseColName, string newTableName, string newColName);


	public:
	api(){ tables = tableList(); }

	//Table Functions
		//Add Table
	bool apiAddTable(string tableName, vector<tuple<string, int>> columns, vector<int>keyPos);
	bool apiAddTable(string tableName, vector<tuple<string, int>> columns, vector<int>keyPos, vector<int> requiredPos);
	bool apiAddTable(string tableName, vector<tuple<string, int>> columns, vector<int> localKeys, vector<tuple<string, string>> foreignPos);
	bool apiAddTable(string tableName, vector<tuple<string, int>> columns, vector<int> localKeys, vector<tuple<string, string>> foreignPos, vector<int> requiredPos);
		//Remove Table
	bool apiRemoveTable(string tableName);
		//Read Table
	vector<tuple<string, int, string, bool, bool>> apiReadTable(string tableName, vector<string> columns);
		//Update Table
	bool apiAddColumn(string tableName, vector<tuple<string, int>> columnNames);
	bool apiRemoveColumn(string tableName, vector<string> columnNames);
	bool apiRenameColumn(string tableName, string columnName, string newName);
	bool apiSetRequired(string tableName, string columnName, bool required);

	//Entry Functions
		//Add Entry
	bool apiAddEntry(string tableName, vector<variant<string, double>> columns);
		//Remove Entry
	bool apiRemoveEntry(string tableName, vector<tuple<string, int, variant<string, double>>> conditions);	//Delete all entries from table where conditions == true
	bool apiRemoveEntry(string tableName, string column, vector<tuple<string, int, variant<string, double>>> conditions);	//Make columns nullptr for all entries where conditions == true
		//Update Entry
	bool apiUpdateEntry(string tableName, string column, double newData);		//Changes value of column to newData for all entries
	bool apiUpdateEntry(string tableName, string column, string newData);
	bool apiUpdateEntry(string tableName, string column, vector<tuple<string, int, variant<string, double>>> conditions, double newData);	//Changes value of column to newData for all entries where if(column[entry] operation compareWith)
	bool apiUpdateEntry(string tableName, string column, vector<tuple<string, int, variant<string, double>>> conditions, string newData);
		//Read Entry
	vector<vector<variant<string, double>>> apiReadEntry(string table, vector<string> displayColumns);
	vector<vector<variant<string, double>>> apiReadEntry(string table, vector<string> displayColumns, vector<tuple<string, int, variant<string, double>>> conditions);
               
	vector<vector<variant<string, double>>> apiReadEntry(vector <string> table, vector<vector<string>> displayColumns);
	vector<vector<variant<string, double>>> apiReadEntry(vector <string> table, vector<vector<string>> displayColumns, vector<vector<tuple<string, int, variant<string, double>>>> conditions);

	//																	table		cols					col		op		compareAgainst										table1	col1			table2	col2
	vector<vector<variant<string, double>>> apiJoinEntry(vector<tuple<string, vector<string>, vector<tuple<string, int, variant<string, double>>>>> columns, vector<tuple<tuple<string, string>, tuple<string, string>>> join);

		//Read Entry (Return String values)

    //Add Index
	bool apiAddIndex(string tableName, string columnName);
  
	bool apiSaveToFile();
	bool apiSaveToFile(string filename);
	bool apiLoadFile(string filename);
};

vector<vector<int>> api::pairEntries(vector<vector<int>> base, string baseTableName, int tablePos, vector<int> acceptedEntries, string baseColName, string newTableName, string newColName){
	//Get elements from join for readability
	string tableName1 = baseTableName;
	string tableName2 = newTableName;
	string col1 = baseColName;
	string col2 = newColName;
	int type1 = tables.getColumnType(tableName1, col1);
	int type2 = tables.getColumnType(tableName2, col2);

	if(-1 == type1 || -1 == type2 || type1 != type2){	//Columns not found or incompatable types
		return {};
	}

	//More readability declarations
	int pos1 = tables.getColumnPosition(tableName1, col1);
	int pos2 = tables.getColumnPosition(tableName2, col2);
	table table1 = tables.getTable(tableName1);
	table table2 = tables.getTable(tableName2);
	vector<vector<int>> paired;
	vector<int> unpaired2 = acceptedEntries;

	//Pairing
	for (int i = 0; i < base.size(); i++){
		bool matched = false;
		for (int j = 0; j < unpaired2.size(); j++){
			if(-1 != base[i][tablePos] && -1 != unpaired2[j]){
				if(table1.entries[base[i][tablePos]]->at(pos1) == table2.entries[unpaired2[j]]->at(pos2)){
					matched = true;
					vector<int> tmp = base[i];	//get row of existing pairs and add unpaired2 pos as new column
					tmp.push_back(unpaired2[j]);
					paired.push_back(tmp);
				}
			}
		}
		if(false == matched){	//No matching value, insert -1 for null
			vector<int> tmp = base[i];
			tmp.push_back(-1);
			paired.push_back(tmp);
		}
	}
	for (int i = 0; i < paired.size(); i++){	//Remove paired elements from unpaired2
		for (int j = 0; j < unpaired2.size(); j++){
			if(paired[i][tablePos + 1] == unpaired2[j]){
				unpaired2.erase(unpaired2.begin() + j);
				break;
			}
		}
	}
	vector<int> nullBase;
	int size = 0;
	if(0 < base.size()){
		size = base[0].size();
	}
	for(int i = 0; i < size; i++){	//Create a -1 base row to append unpaired2 to
		nullBase.push_back(-1);
	}
	for(int i = unpaired2.size() - 1; i >= 0; i--){
		vector<int> tmp = nullBase;
		tmp.push_back(unpaired2[i]);
		paired.push_back(tmp);
	}

	return paired;
}


//																		table		cols					col		op		compareAgainst										table1	  col1			table2	col2
vector<vector<variant<string, double>>> api::apiJoinEntry(vector<tuple<string, vector<string>, vector<tuple<string, int, variant<string, double>>>>> columns, vector<tuple<tuple<string, string>, tuple<string, string>>> join){
	vector<string> tableNames = vector<string>();

	//Get accepted entries for all columns
//	vector<vector<int>> acceptedEntries = vector<vector<int>>();
/*	for(int i = 0; i < columns.size(); i++){
		string tableName = get<0>(columns[i]);
		//Create a list of all tables in use
		for(int j = 0; j < tableNames.size(); j++){
			if(tableName == tableNames[j]){
				return {};	//Repeated tables is not allowed, could fix but would run slower than just enforcing unique tables
			}
		}
		tableNames.push_back({tableName, false});
		acceptedEntries.push_back(getAcceptedEntries(tableName, get<2>(columns[i])));
	}
*/
	//Setup the first join
	//			baseTbl	tblPos	accepted	baseCol	newTbl	newCol
	vector<tuple<string, int, vector<int>, string, string, string>> args;
	vector<vector<int>> joined = {};
	if(join.size() > 0){
		string baseTable = get<0>(get<0>(join[0]));
		tableNames.push_back(baseTable);
		int tablePos = 0;
		vector<tuple<string, int, variant<string, double>>> conditions = {};
		string baseCol = get<1>(get<0>(join[0]));
		for(int i = 0; i < columns.size(); i++){	//Find any conditions for table1
			if(get<0>(columns[i]) == baseTable){
				conditions = get<2>(columns[i]);
				break;
			}
		}
		vector<int> tmp = getAcceptedEntries(tables.getTable(baseTable), conditions);
		for(int i = 0; i < tmp.size(); i++){
			joined.push_back({tmp[i]});
		}

		conditions = {};
		string newTable = get<0>(get<1>(join[0]));	
		tableNames.push_back(newTable);
		for(int i = 0; i < columns.size(); i++){	//Find any conditions for table2
			if(get<0>(columns[i]) == newTable){
				conditions = get<2>(columns[i]);
				break;
			}
		}
		vector<int> accepted = getAcceptedEntries(tables.getTable(newTable), conditions);
		string newCol = get<1>(get<1>(join[0]));
		args = {{baseTable, tablePos, accepted, baseCol, newTable, newCol}};
	}

	//Get accepted entries for all columns in the order found in join
	for(int i = 1; i < join.size(); i++){
		string baseTable = get<0>(get<0>(join[i]));
		int tablePos;
		vector<int> accepted;
		string baseCol = get<1>(get<0>(join[i]));
		string newTable = get<0>(get<1>(join[i]));
		string newCol = get<1>(get<1>(join[i]));

		bool found = false;
		bool foundBase = false;
		string tableName = newTable;
		for(int j = 0; j < tableNames.size(); j++){
			if(false == foundBase && baseTable == tableNames[j]){
				foundBase = true;
				tablePos = j;	//Tables are merged in the same order as tableNames are added to the list
			}
			if(tableName == tableNames[j]){
				return {};	//Repeated tables is not allowed, could fix but would run slower than just enforcing unique tables
			}
		}
		if(false == foundBase){
			return {};		//Base table must be a previously merged table
		}
		for(int j = 0; j < columns.size(); j++){
			if(tableName == get<0>(columns[j])){	//Table has conditions
				found = true;
				accepted = getAcceptedEntries(tables.getTable(tableName), get<2>(columns[j]));
				break;
			}
		}
		if(false == found){		//Table has no conditions, get every entry
			accepted = getAcceptedEntries(tables.getTable(tableName), {});
		}
		tableNames.push_back(tableName);
		args.push_back({baseTable, tablePos, accepted, baseCol, newTable, newCol});
	}

	//Join accepted entries into new table format
	for(int i = 0; i < join.size(); i++){
		joined = pairEntries(joined, get<0>(args[i]), get<1>(args[i]), get<2>(args[i]), get<3>(args[i]), get<4>(args[i]), get<5>(args[i]));
	}

	//get columns to output and the table they go to
	vector<vector<string>> outputCols;
	for(int i = 0; i < tableNames.size(); i++){
		bool found = false;
		for(int j = 0; j < columns.size(); j++){
			if(get<0>(columns[j]) == tableNames[i]){	//If columns were given, add chosen columns to output
				outputCols.push_back(get<1>(columns[j]));
				found = true;
				break;
			}
		}
		if(false == found){		//If columns were not given, add all columns of table to output
			table tmp = tables.getTable(tableNames[i]);
			vector<string> tmpColNames = {};
			for(int j = 0; j < tmp.columns.size(); j++){
				tmpColNames.push_back(get<0>(tmp.columns[j]));
			}
			outputCols.push_back(tmpColNames);
		}
	}
	vector<vector<variant<string, double>>> comboTable = vector<vector<variant<string, double>>>(joined.size());
	if(joined.size() > 0){
		for(int j = 0; j < joined[0].size(); j++){		//Column
			vector<variant<string, double>> column;
			vector<string> columnNames = outputCols[j];
			table tmp = tables.getTable(tableNames[j]);
			vector<int> colPos = {};
			for(int i = 0; i < columnNames.size(); i++){	//Find colPos for current column
				for(int k = 0; k < tmp.columns.size(); k++){
					if(get<0>(tmp.columns[k]) == columnNames[i]){
						colPos.push_back(k);
						break;
					}
				}
			}
			for(int i = 0; i < joined.size(); i++){		//Row
				if(-1 == joined[i][j]){	//Table is null at position
					for(int k = 0; k < outputCols[j].size(); k++){	//Loop through the output columns
						int type = get<1>(tmp.columns[colPos[k]]);
						if(0 == type){
							comboTable[i].push_back("NULL");
						}
						else if(1 == type){
							comboTable[i].push_back(nullDouble);
						}
					}
				}
				else{
					for(int k = 0; k < outputCols[j].size(); k++){	//Loop through the output columns
						comboTable[i].push_back(tmp.entries[joined[i][j]]->at(colPos[k]));
					}
				}
			}
		}
	}

	return comboTable;

/*
	vector<tuple<tuple<string, string>, vector<tuple<int, int>>>> joinedEntryPos;
	//Join accepted entries into new table format
	for(int i = 0; i < join.size(); i++){
		//Get elements from join for readability
		string tableName1 = get<0>(get<0>(join[i]));
		string col1 = get<0>(get<1>(join[i]));
		string tableName2 = get<1>(get<0>(join[i]));
		string col2 = get<1>(get<1>(join[i]));
		int type1 = tables.getColumnType(tableName1, col1);
		int type2 = tables.getColumnType(tableName2, col2);

		if(-1 == type1 || -1 == type2 || type1 != type2){	//Columns not found or incompatable types
			return {};
		}

		//More readability declarations
		int pos1 = tables.getColumnPosition(tableName1, col1);
		int pos2 = tables.getColumnPosition(tableName2, col2);
		table table1 = tables.getTable(tableName1);
		table table2 = tables.getTable(tableName2);
		vector<tuple<int, int>> paired;
		vector<int>	unpaired1;
		vector<int> unpaired2;
		
		//Get the entry positions to be paired
		bool found = false;
		for(int j = 0; j < columns.size(); j++){
			if(tableName1 == get<0>(columns[j])){
				found == true;
				unpaired1 = acceptedEntries[j];	//Just get the postions that match the conditions
				break;
			}
		}
		if(false == found){	//Table1 did not have any conditions
			for (int j = 0; j < table1.entries.size(); j++){
				unpaired1.push_back(j);
			}
		}
		found = false;
		for(int j = 0; j < columns.size(); j++){
			if(tableName2 == get<0>(columns[j])){
				found == true;
				unpaired2 = acceptedEntries[j];	//Just get the postions that match the conditions
				break;
			}
		}
		if(false == found){		//Table2 did not have any conditions
			for (int j = 0; j < table2.entries.size(); j++){
				unpaired2.push_back(j);
			}
		}

		vector<tuple<int, int>>;
		//Pairing
		for (int j = unpaired1.size() - 1; j >= 0; j--){	//Removing from the back of a vector should be faster
			bool matched = false;
			for (int k = unpaired2.size() - 1; k >= 0; k--){
				if(table1.entries[unpaired1[j]][pos1] == table2.entries[unpaired2[k]][pos2]){
					matched = true;
					paired.push_back({unpaired1[j], unpaired2[k]});
				}
			}
			if(false == matched){
				paired.push_back({unpaired1[j], -1});
			}
			unpaired1.pop_back();	//Not removing paired elements from unpaired2 because they might be matched with later elements from unpaired1
		}
		for (int j = 0; j < paired.size(); j++){	//Remove paired elements from unpaired2
			for (int k = unpaired2.size() - 1; k >= 0; k--){
				if(get<1>(paired[j]) == unpaired2[k]){
					unpaired2.erase(unpaired2.begin() + k);
					break;
				}
			}
		}
		for(int j = unpaired2.size() - 1; j >= 0; j--){
			paired.push_back({-1, unpaired2[j]});	//Don't need to pop_back since unpaired is not used after this
		}
		joinedEntryPos.push_back({{tableName1, tableName2}, paired});
	}
	*/
}

bool api::apiLoadFile(string filename){
		return tables.loadTables(filename);
}

bool api::apiSaveToFile(){
		return apiSaveToFile("tmpName.json");
}

bool api::apiSaveToFile(string filename){
		return tables.writeTables(filename);
}

bool api::isRequired(table* workingTable, string column){
	for(int i = 0; i < workingTable->required.size(); i++){
		if(column == workingTable->required[i])
			return true;
	}
	return false;
}

bool api::apiRemoveEntry(string tableName, vector<tuple<string, int, variant<string, double>>> conditions){
	return apiRemoveEntry(tableName, "", conditions);
}

bool api::apiRemoveEntry(string tableName, string column, vector<tuple<string, int, variant<string, double>>> conditions){
	table* t = tables.getTablePointer(tableName);
	if(nullptr == t){
		return false;
	}
	int columnPos;
	bool wholeEntry = false;
	if("" != column){
		wholeEntry = true;
		columnPos = tables.getColumnPosition(tableName, column);
		if(-1 == columnPos)
			return false;
		for(int i = 0; i < t->keys.size(); i++){
			if(get<1>(t->keys[i]) == column){	//Trying to delete a key value
				return false;
			}
		}
	}
	vector<int> accepted = getAcceptedEntries(*t, conditions);
	for(int i = accepted.size() - 1; i >= 0; i--){			//Iterating backwards to avoid changing index positions
		int entry = accepted[i];
		if(true == wholeEntry){
			if(0 == get<1>(t->columns[columnPos])){
				t->entries[entry]->at(columnPos) = "";		//String can't be nullptr
			}
			else{
				t->entries[entry]->at(columnPos) = nullptr;
			}
		}
		else{
			delete t->entries[entry];						//Each entry is a new object
			t->entries.erase(t->entries.begin() + entry);	//Remove the entry from the list
		}
	}
	return true;
}

bool api::keyUsed(table* workingTable, int columnPos, variant<string, double> data){
	bool keyColumn = false;
	for(int i = 0; i < workingTable->keys.size(); i++){
		if(get<0>(workingTable->columns[columnPos]) == get<1>(workingTable->keys[i]))
			keyColumn = true;
	}
	if(false == keyColumn)	//The chosen column is not a key column
		return false;

	int type = get<1>(workingTable->columns[columnPos]);
	string stringData;
	double doubleData;
	if(0 == type){
		stringData = get<string>(data);
	}
	else if(1 == type){
		doubleData = get<double>(data);
	}

	for(int i = 0; i < workingTable->entries.size(); i++){
		switch(type){
			case 0:
				if(stringData == get<string>(workingTable->entries[i]->at(columnPos)))	//String entry already exists in column
					return true;
				break;
			case 1:
				if(doubleData == get<double>(workingTable->entries[i]->at(columnPos)))	//String entry already exists in column
					return true;
				break;
		}
	}

	return false;
}

bool api::apiAddEntry(string tableName, vector<variant<string, double>> columns){
	table* t = tables.getTablePointer(tableName);
	if(nullptr == t)
		return false;
	vector<variant<string, double>> *entry = new vector<variant<string, double>>();
	bool valid = true;
	for(int i = 0; i < columns.size(); i++){
		variant<string, double> colVal;
		if(holds_alternative<string>(columns[i])){	//String columns
			if(0 != get<1>(t->columns[i]))	//Incompatable column type and column value
				return false;
			if(get<string>(columns[i]) == "" && true == isRequired(t, get<0>(t->columns[i])))	//Attempting to put "" into required column
				return false;
			colVal = get<string>(columns[i]);
		}
		else if(holds_alternative<double>(columns[i])){	//Double columns
			if(1 != get<1>(t->columns[i]))	//Incompatable column type and column value
				return false;
			colVal = get<double>(columns[i]);
		}
		if(false == keyUsed(t, i, colVal)){	//Duplicate values are not allowed in key columns
			entry->push_back(colVal);
		}
		else{
			valid = false;
			break;
		}
	}
	if(true == valid){
		t->entries.push_back(entry);
		return true;
	}
	else{
		delete entry;
		return false;
	}
}


bool api::apiAddTable(string tableName, vector<tuple<string, int>> columns, vector<int> keyPos){
	vector<int> tmp;
	return apiAddTable(tableName, columns, keyPos, tmp);
}

bool api::apiAddTable(string tableName, vector<tuple<string, int>> columns, vector<int> keyPos, vector<int> requiredPos){
	if(0 == keyPos.size())
		return false;		//At least one key is mandatory
	return apiAddTable(tableName, columns, keyPos, {}, requiredPos);

}

bool api::apiAddTable(string tableName, vector<tuple<string, int>> columns, vector<int> localKeys, vector<tuple<string, string>> foreignPos){
	return apiAddTable(tableName, columns, localKeys, foreignPos, {});
}

bool api::apiAddTable(string tableName, vector<tuple<string, int>> columns, vector<int> localKeys, vector<tuple<string, string>> foreignPos, vector<int> requiredPos){
	table tmp = tables.getTable(tableName);
	if("error" != tmp.name)	//Name already in use
		return false;

	for(int i = 0; i < localKeys.size(); i++){
		if(0 > localKeys[i] || columns.size() <= localKeys[i])		//Check key column(s) point to actual columns
			return false;
	}
	vector<string> tmpRequired;
	for(int i = 0; i < requiredPos.size(); i++){
		if(0 > requiredPos[i] || columns.size() <= requiredPos[i])		//Check required column(s) point to actual columns
			return false;
		else
			tmpRequired.push_back(get<0>(columns[i]));
	}
	for(int i = 0; i < columns.size(); i++){						//Check for repeated column names O(n!) runtime
		for(int j = 0; j < i; j++){
			if(columns[i] == columns[j])
				return false;
		}
	}

	int ran;	//ran will be checked against later
	for(ran = 0; ran < foreignPos.size(); ran++){
		bool found = false;
		for(int j = 0; j < columns.size(); j++){
			if(get<1>(foreignPos[ran]) == get<0>(columns[j])){
				found = true;
				break;
			}
		}
		if(false == found)
			break;
		
		if(false == tables.addForeignKey(get<0>(foreignPos[ran]), get<1>(foreignPos[ran])))
			break;
	}

	if(ran < foreignPos.size()){		//Undoing addForeignKeys that ran before failure
		for(int i = 0; i < ran; i++){
			tables.removeForeignKey(get<0>(foreignPos[i]), get<1>(foreignPos[i]));
		}
		return false;
	}

	vector<tuple<string, string>> tmpKeys;
	for(int i = 0; i < localKeys.size(); i++){
		tmpKeys.push_back({"", get<0>(columns[localKeys[i]])});
	}
	for(int i = 0; i < foreignPos.size(); i++){
		tmpKeys.push_back(foreignPos[i]);
	}

	for(int i = 0; i < tmpKeys.size(); i++){			//Makes key columns required columns
		bool found = false;
		for(int j = 0; j < requiredPos.size(); j++){
			if(get<1>(tmpKeys[i]) == tmpRequired[j]){
				found = true;
				break;
			}
		}
		if(false == found){
			tmpRequired.push_back(get<1>(tmpKeys[i]));
		}
	}

	table *newTable = new table(tableName);
	newTable->columns = columns;
	newTable->keys = tmpKeys;
	newTable->required = tmpRequired;

	tables.addTable(newTable);

	return true;
}

bool api::apiRemoveTable(string table){
	return tables.removeTable(table);
}

//returns {columnName, columnType, ForeignKeyTableName or "", key, columnRequired}
vector<tuple<string, int, string, bool, bool>> api::apiReadTable(string tableName, vector<string> columns){
	// Get the table object
	table t = tables.getTable(tableName);

	vector<tuple<string, int, string, bool, bool>> result;

	// Check if the table exists
	if (t.name == "error") {
	    return result;  // Return an empty vector if the table does not exist
	}
	
	bool runAll = false;
	if(0 == columns.size())
		runAll = true;

	for(int colPos = 0; colPos < columns.size(); colPos++){
		bool found = false;
		for(int i = 0; i < t.columns.size(); i++){
			if(get<0>(t.columns[i]) == columns[colPos]){
				bool key = false;
				string foreign = "";
				bool required = false;
				for(int j = 0; j < t.keys.size(); j++){
					if(get<1>(t.keys[j]) == columns[colPos]){
						key = true;
						foreign = get<0>(t.keys[j]);
						break;
					}
				}
				for(int j = 0; j < t.required.size(); j++){
					if(t.required[j] == columns[colPos]){
						required = true;
						break;
					}
				}
				found = true;
				result.push_back({get<0>(t.columns[i]), get<1>(t.columns[i]), foreign, key, required});
				break;
			}
		}
		if(false == found){
			result.clear();
			return result;		//if it gets here the column was not found
		}
	}

	return result;
}

bool api::apiAddColumn(string tableName, vector<tuple<string, int>> columnNames){
	table* tempTable = tables.getTablePointer(tableName);
	if (nullptr == tempTable) {
	    return false;
	}
	for(int i=0; i<columnNames.size(); i++){
		//Error handling needed here
		tempTable->columns.push_back(columnNames[i]);
	}
	return true;
}

bool api::apiRemoveColumn(string tableName, vector<string> columnNames){
	table* tempTable = tables.getTablePointer(tableName);
	if (nullptr == tempTable) {
	    return false;
	}
	int index = -1;
	for(int i=0; i<columnNames.size(); i++){
		if(get<0>(tempTable->columns[i]) == columnNames[i]){
			tempTable->columns.erase(tempTable->columns.begin() + i);
			return true;
		}
	}
	if(index == -1){
		return false;
	}
	// I think I'm supposed to erase it from other vectors as well but I'm not sure which ones
	return false;
}

bool api::apiRenameColumn(string tableName, string columnName, string newName){
	table* tempTable = tables.getTablePointer(tableName);
	if (nullptr == tempTable) {
	    return false;
	}
	for(int i=0; i<tempTable->columns.size(); i++){
		if(get<0>(tempTable->columns[i]) == columnName){
			get<0>(tempTable->columns[i]) = newName;
			return true;
		}
	}
	return false;
}

bool api::apiSetRequired(string tableName, string columnName, bool required){
	table* tempTable = tables.getTablePointer(tableName);
	if (nullptr == tempTable) {
	    return false;
	}
	for(int i=0; i<tempTable->columns.size(); i++){
		if(get<0>(tempTable->columns[i]) == columnName){
			tempTable->required[i] = required;
			return true;
		}
	}
	return false;
}

bool api::compare(variant<string, double> left, int op, variant<string, double> right){		//Assumes types are the same, check before calling
	    switch (op) {
            case 0:  
                return left == right;
            case 1:  
                return left > right;
            case 2:  
                return left >= right;
            case 3:  
                return left < right;
            case 4:  
                return left <= right;
            default:
                return false;  
        }
}



vector<int> api::getAcceptedEntries(table workingTable, vector<tuple<string, int, variant<string, double>>> conditions){
	vector<int> accepted;
	for(int i = 0; i < workingTable.entries.size(); i++){
		accepted.push_back(i);
	}

	if(0 == conditions.size()){		//no conditions means return all entries
		return accepted;
	}

	string columnName = "";
	int colPos = 0;
	for(int i = 0; i < accepted.size(); i++){
		bool removed = false;
		int entry = accepted[i];
		for(int j = 0; j < conditions.size(); j++){
			if(columnName != get<0>(conditions[j])){
				columnName = get<0>(conditions[j]);
				colPos = tables.getColumnPosition(workingTable.name, columnName);	//Could improve performance by making new getColumn[.]s that takes tables
				if(-1 == colPos)
					return {};
			}
			int operation = get<1>(conditions[j]);
			variant<string, double> compareWith = get<2>(conditions[j]);
			int type = get<1>(workingTable.columns[colPos]);
			switch(type){
				case 0:
					if(false == holds_alternative<string>(compareWith))	//Incompatable column type and comparison value
						return {};
					if(false == compare(get<string>(workingTable.entries[entry]->at(colPos)), operation, get<string>(compareWith))){
						accepted.erase(accepted.begin() + i);
						i--;
						removed = true;
					}
					break;
				case 1:
					if(false == holds_alternative<double>(compareWith))	//Incompatable column type and comparison value
						return {};
					if(false == compare(get<double>(workingTable.entries[entry]->at(colPos)), operation, get<double>(compareWith))){
						accepted.erase(accepted.begin() + i);
						i--;
						removed = true;
					}
					break;
				default:
					return {};
			}
			if(true == removed)
				break;
		}
	}
	return accepted;
}

bool api::apiUpdateEntry(string tableName, string column, double newData) {
    return apiUpdateEntry(tableName, column, {}, newData);
}

bool api::apiUpdateEntry(string tableName, string column, string newData) {
    return apiUpdateEntry(tableName, column, {}, newData);
}

bool api::apiUpdateEntry(string tableName, string column, vector<tuple<string, int, variant<string, double>>> conditions, double newData) {
    table* t = tables.getTablePointer(tableName);

    if (nullptr == t) {
        return false;
    }

    int columnPos = -1;
    for (int i = 0; i < t->columns.size(); i++) {
        if (get<0>(t->columns[i]) == column) {
            columnPos = i;
            break;
        }
    }

    if (columnPos == -1) {
        return false;  
    }

    if(tables.getColumnType(tableName, column) != 1){
        return false;
    }

	for(int i = 0; i < t->keys.size(); i++){
		if(column == get<1>(t->keys[i])){
			if(true == keyUsed(t, columnPos, newData)){		//Check for duplicate key value
				return false;
			}
			break;
		}
	}

	vector<int> accepted = getAcceptedEntries(*t, conditions);
	for(int i = 0; i < accepted.size(); i++){
		t->entries[accepted[i]]->at(columnPos) = newData;
	}

    return true;
}

bool api::apiUpdateEntry(string tableName, string column, vector<tuple<string, int, variant<string, double>>> conditions, string newData){
    table* t = tables.getTablePointer(tableName);
	
    if (nullptr == t) {
        return false;
    }

    int columnPos = -1;
    for (int i = 0; i < t->columns.size(); i++) {
        if (get<0>(t->columns[i]) == column) {
            columnPos = i;
            break;
        }
    }

    if (columnPos == -1) {
        return false;  
    }

    if(tables.getColumnType(tableName, column) != 0){
        return false;
    }

	for(int i = 0; i < t->keys.size(); i++){
		if(column == get<1>(t->keys[i])){
			if(true == keyUsed(t, columnPos, newData)){		//Check for duplicate key value
				return false;
			}
			break;
		}
	}
	
	vector<int> accepted = getAcceptedEntries(*t, conditions);
	for(int i = 0; i < accepted.size(); i++){
		t->entries[accepted[i]]->at(columnPos) = newData;
	}

    return true;
}

vector<vector<variant<string, double>>> api::apiReadEntry(string tableName, vector<string> displayColumns) {
    return apiReadEntry(tableName, displayColumns, {});
}

vector<vector<variant<string, double>>> api::apiReadEntry(string tableName, vector<string> displayColumns, vector<tuple<string, int, variant<string, double>>> conditions) {
    table* t = tables.getTablePointer(tableName);
    vector<vector<variant<string, double>>> result;
	if(nullptr == t){
		return {};
	}

	vector<int> columnPositions;
	for (string col : displayColumns) {
	    for (int i = 0; i < t->columns.size(); i++) {
	        if (col == get<0>(t->columns[i])) {
	            columnPositions.push_back(i);
	            break;
	        }
	    }
	}
	vector<int> accepted = getAcceptedEntries(*t, conditions);
	for(int i = 0; i < accepted.size(); i++){
		int entry = accepted[i];
		vector<variant<string, double>> entryResult;
		for (int pos : columnPositions) {
			entryResult.push_back(t->entries[entry]->at(pos));
		}
		result.push_back(entryResult);
	}
    return result;
}

vector<vector<variant<string, double>>> api::apiReadEntry(vector <string> table, vector<vector<string>> displayColumns){
	return apiReadEntry(table, displayColumns, {});
}

vector<vector<variant<string, double>>> api::apiReadEntry(vector <string> table, vector<vector<string>> displayColumns, vector<vector<tuple<string, int, variant<string, double>>>> conditions){
	if(table.size() != displayColumns.size()){
		return {};
	}
	bool ans = false;
	if(table.size() <= conditions.size()){
		ans = true;
	}
	vector<vector<variant<string, double>>> returnEntries, temp;
	for(int i = 0; i < table.size(); ++i){
		vector<tuple<string, int, variant<string, double>>> condTemp = {};
		if(ans == true){
			condTemp = conditions[i];	
		}
		temp = apiReadEntry(table[i], displayColumns[i], condTemp);
		for(int j = 0; j < temp.size(); ++j){
			returnEntries.push_back(temp[j]);
		}
	}
	return returnEntries;
}

bool api::apiAddIndex(string tableName, string columnName){
    // Find the table with the given tableName
    table* workingTable = tables.getTablePointer(tableName);
    if (workingTable == nullptr) {
        return false;  // table not found
    }


    // Find the column with the given columnName in the table
    int columnPos = -1;
    for (int i = 0; i < workingTable->columns.size(); i++) {
        if (get<0>(workingTable->columns[i]) == columnName) {
            columnPos = i;
            break;
        }
    }

    if (columnPos == -1) {
        return false;  // column not found
    }

    // Check if an index already exists for the column
    for (int i = 0; i < workingTable->indexes.size(); i++) {
        if (get<0>(workingTable->indexes[i]) == columnName) {
            return false;  // index already exists
        }
    }

    // Sort the entries in the column
    sort(workingTable->entries.begin(), workingTable->entries.end(),
         [columnPos](const vector<variant<string, double>>* a, const vector<variant<string, double>>* b) {
             return a->at(columnPos) < b->at(columnPos);
         });

    // Store the positions of the entries in the indexes vector
    vector<int> indexPositions;
    for (int i = 0; i < workingTable->entries.size(); i++) {
        indexPositions.push_back(i);
    }
    workingTable->indexes.push_back(make_tuple(columnName, indexPositions));

    return true;
}


