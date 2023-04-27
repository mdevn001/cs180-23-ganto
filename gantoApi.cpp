#include "tableList.cpp"
class api{
	private:
	tableList tables;

	public:
api(){
	tables = tableList();
}


bool addTable(string tableName, vector<tuple<string, int>> columns, vector<int> keyPos){
	vector<int> tmp;
	return addTable(tableName, columns, keyPos, tmp);
}

bool addTable(string tableName, vector<tuple<string, int>> columns, vector<int> keyPos, vector<int> requiredPos){
	table tmp = tables.getTable(tableName);
	if("error" != tmp.name)	//Name already in use
		return false;
		
	table *newTable = new table(tableName);
	newTable->columns = columns;		//Checking for column name re-use here would be expensive O(n!)

	if(0 == keyPos.size())
		return false;		//At least one key is mandatory
	for(int i = 0; i < keyPos.size(); i++){
		if(0 > keyPos[i] || columns.size() <= keyPos[i])		//Check key column(s) point to actual columns
			return false;
	}
	newTable->keys = keyPos;
	for(int i = 0; i < requiredPos.size(); i++){
		if(0 > requiredPos[i] || columns.size() <= requiredPos[i])		//Check required column(s) point to actual columns
			return false;
	}

	tables.addTable(newTable);

	return true;
}

bool apiRemoveTable(string table){
	return tables.removeTable(table);
}

tuple<vector<string>, vector<vector<int>>> apiReadTable(string tableName, vector<string> columns){
	// Get the table object
	table t = tables.getTable(tableName);
	// Check if the table exists
	if (t.name == "error") {
	    return {};  // Return an empty vector if the table does not exist
	}
	vector<int> tmpInt;
	vector<string> tmpString;
	tmpString.push_back(t.name);
	for(int i = 0; i < t.columns.size(); i++){
		tmpString.push_back(get<0>(t.columns[i]));
		tmpInt.push_back(get<1>(t.columns[i]));
	}
	vector<vector<int>> comboInt;
	comboInt.push_back(tmpInt);
	comboInt.push_back(t.keys);
	comboInt.push_back(t.required);

	return {tmpString, comboInt};
}

bool apiAddColumn(string tableName, vector<tuple<string, int>> columnNames){
	table* tempTable = tables.getTablePointer(tableName);
	if (tempTable->name == "error") {
	    return false;
	}
	for(int i=0; i<columnNames.size(); i++){
		//Error handling needed here
		tempTable->columns.push_back(columnNames[i]);
	}
	return true;
}

bool apiRemoveColumn(string tableName, vector<string> columnNames){
	table* tempTable = tables.getTablePointer(tableName);
	if (tempTable->name == "error") {
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

bool apiRenameColumn(string tableName, string columnName, string newName){
	table* tempTable = tables.getTablePointer(tableName);
	if (tempTable->name == "error") {
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

bool apiSetRequired(string tableName, string columnName, bool required){
	table* tempTable = tables.getTablePointer(tableName);
	if (tempTable->name == "error") {
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
};

vector<vector<variant<string, double>>> apiReadEntry(string tableName, vector<string> displayColumns) {
    table* t = tableList.getTablePointer(tableName);
    vector<vector<variant<string, double>>> result;
    if (t != nullptr) {
        vector<int> columnPositions;
        for (string col : displayColumns) {
            for (int i = 0; i < t->columns.size(); i++) {
                if (col == get<0>(t->columns[i])) {
                    columnPositions.push_back(i);
                    break;
                }
            }
        }
        for (int i = 0; i < t->entries.size(); i++) {
            vector<variant<string, double>> entryResult;
            for (int pos : columnPositions) {
                entryResult.push_back(t->entries[i]->at(pos));
            }
            result.push_back(entryResult);
        }
    }
    return result;
}

vector<vector<variant<string, double>>> api::apiReadEntry(string tableName, vector<string> displayColumns, vector<tuple<string, int, variant<string, double>>> conditions) {
    table* t = tableList.getTablePointer(tableName);
    vector<vector<variant<string, double>>> result;
    if (t != nullptr) {
        vector<int> columnPositions;
        for (string col : displayColumns) {
            for (int i = 0; i < t->columns.size(); i++) {
                if (col == get<0>(t->columns[i])) {
                    columnPositions.push_back(i);
                    break;
                }
            }
        }
        for (int i = 0; i < t->entries.size(); i++) {
            bool isEntryValid = true;
            for (auto condition : conditions) {
                int colPos = -1;
                for (int i = 0; i < t->columns.size(); i++) {
                    if (get<0>(t->columns[i]) == get<0>(condition)) {
                        colPos = i;
                        break;
                    }
                }
                if (colPos == -1) {
                    isEntryValid = false;
                    break;
                }
                int op = get<1>(condition);
                variant<string, double> val = get<2>(condition);
                variant<string, double> entryVal = t->entries[i]->at(colPos);
                bool isConditionMet = false;
                switch (op) {
                    case 0:
                        isConditionMet = (entryVal == val);
                        break;
                    case 1:
                        isConditionMet = (holds_alternative<double>(entryVal) && get<double>(entryVal) > get<double>(val));
                        break;
                    case 2:
                        isConditionMet = (holds_alternative<double>(entryVal) && get<double>(entryVal) >= get<double>(val));
                        break;
                    case 3:
                        isConditionMet = (holds_alternative<double>(entryVal) && get<double>(entryVal) < get<double>(val));
                        break;
                    case 4:
                        isConditionMet = (holds_alternative<double>(entryVal) && get<double>(entryVal) <= get<double>(val));
                        break;
                    default:
                        break;
                }
                if (!isConditionMet) {
                    isEntryValid = false;
                    break;
                }
            }
            if (isEntryValid) {
                vector<variant<string, double>> entryResult;
                for (int pos : columnPositions) {
                    entryResult.push_back(t->entries[i]->at(pos));
                }
                result.push_back(entryResult);
            }
        }
    }
    return result;

