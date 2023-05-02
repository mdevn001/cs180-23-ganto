#include "gantoApi.cpp"
#include <iostream>
api t = api();
bool integrationTest = false;	//Use this to turn on/off integration tests

void addTableTest(){
	cout << "Add Table Tests" << endl;
	vector<tuple<string, int>> columns = {{"name", 0}, {"age", 1}, {"location", 0}};
	vector<int> keys = {0};
	vector<int> requiredPos;
	cout << "\tBasic Test: ";
	if(true == t.apiAddTable("test", columns, keys))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	columns = {make_tuple("Name", 0), make_tuple("Age", 1)};
	cout << "\tValidTable: ";
	if(true == t.apiAddTable("People", columns, keys))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	columns = {make_tuple("Name", 0), make_tuple("Age", 1)};
	cout << "\tInvalidTable: ";
	if(false == t.apiAddTable("People", columns, keys))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;
	
	
	columns = {make_tuple("Name", 0), make_tuple("Age", 1)};
	cout << "\tDuplicateTable: ";
	if(false == t.apiAddTable("People", columns, keys))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	cout << "\tKeyIsForeign: ";
	if(true == t.apiAddTable("Foreign", {{"name", 0}, {"birthPlace", 0}}, {}, {make_tuple("People", "name")}))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;
}

void updateTableTest(){
	cout << "Update Table Tests" << endl;
	vector<tuple<string, int>> columns = {{"time", 1}, {"distance", 1}, {"personality", 0}};
	vector<int> keys = {0};
	vector<int> requiredPos;
	cout << "\tAdd Column Test: ";
	if(true == t.apiAddColumn("test", columns))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	/*
	cout << "\tColumnUsedAsForeignKey: ";
	if(false == t.apiAddTable("People", columns, keys))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;
	*/
}

void deleteTableTest(){
	cout << "Delete Table Tests" << endl;
	vector<tuple<string, int>> columns = {{"name", 0}, {"age", 1}, {"location", 0}};
	vector<int> keys = {0};
	vector<int> requiredPos;
	cout << "\tBasic Test: ";
	if(true == t.apiRemoveTable("test"))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;
	/*
	cout << "\tTableUsedAsForeignKey: ";
	if(false == t.apiAddTable("People", columns, keys))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	cout << "\tTablePosUpdated: ";
	if(false == t.apiAddTable("People", columns, keys))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;
	*/
}

void readTableTest(){
	cout << "Read Table Tests" << endl;

	vector<tuple<string, int>> columns = {{"name", 0}, {"age", 1}, {"location", 0}};
	vector<int> keys = {0};
	vector<int> requiredPos;
	t.apiAddTable("test", columns, keys);

	vector<string> columnChoice = {"column1", "column2"};
	cout << "\tTableDoesNotExist: ";
	if(get<0>(t.apiReadTable("nonexistentTable", columnChoice)).empty() && get<1>(t.apiReadTable("nonexistentTable", columnChoice)).empty())
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	columns = {make_tuple("Name", 0), make_tuple("Age", 1)};
	cout << "\tColumnsDoNotExist: ";
	if(false == t.apiAddTable("People", columns, keys))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

}


void addEntryTest(){
	t.apiAddTable("AddEntry", {{"String", 0}, {"Double", 1}}, {0});
	cout << "Add Entry Tests" << endl;
	cout << "\tValidEntry: ";
	if(true == t.apiAddEntry("AddEntry", {{"Valid", 1.0}}))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	cout << "\tDuplicateKeyValue: ";
	if(false == t.apiAddEntry("AddEntry", {{"Valid", 2.0}}))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

/*		nullptr doesn't work with variants?
	cout << "\tNullKeyValue: ";
	if(false == t.apiAddEntry("AddEntry", {nullptr, 3.0}))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	t.apiSetRequired("AddEntry", "Double", true);
	cout << "\tNullRequiredValue: ";
	if(false == t.apiAddEntry("AddEntry", {"AlsoValid", nullptr}))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;
*/

}

void updateEntryTest(){
	t.apiAddTable("UpdateEntry", {{"String", 0}, {"Double", 1}, {"Third Thing", 0}}, {1});
	vector<vector<variant<string, double>>> returned;
	vector<vector<variant<string, double>>> entries;
	entries.push_back({{"entry1", 10.0, "#1"}});
	entries.push_back({{"entry2", 20.0, "#2"}});
	entries.push_back({{"entry3", 10.3, "#3"}});
	entries.push_back({{"entry4", 14.0, "#4"}});
	entries.push_back({{"entry5", 55.0, "#5"}});
	entries.push_back({{"entry6", 66.6, "#6"}});
	for(int i = 0; i < entries.size(); i++){
		t.apiAddEntry("UpdateEntry", entries[i]);
	}



	cout << "update Entry Tests" << endl;

	cout << "\tNoConditions(string): ";
	if(true == t.apiUpdateEntry("UpdateEntry", "String", "joe"))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	cout << "\tOneCondition: ";
	if(true == t.apiUpdateEntry("UpdateEntry", "String", {{"Third Thing", 0, "#2"}}, "bob"))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	cout << "\tDuplicateKeyValue: ";
	if(false == t.apiUpdateEntry("UpdateEntry", "Double", 10))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	/*
	cout << "\tNullKeyValue: ";
	if(true == t.apiUpdateEntry("UpdateEntry", "Double", nullptr))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;
	*/

	cout << "\tMultipleConditions: ";
	if(true == t.apiUpdateEntry("UpdateEntry", "String", {{"Double", 1, 10.0}, {"String", 0, "joe"}}, "zoe"))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;
}

/*
void deleteEntryTest(){
	t.apiAddTable("DeleteEntry", {{"String", 0}, {"Double", 1}, {"Third Thing", 0}}, {1});
	vector<vector<variant<string, double>>> returned;
	vector<vector<variant<string, double>>> entries;
	entries.push_back({{"entry1", 10.0, "#1"}});
	entries.push_back({{"entry2", 20.0, "#2"}});
	entries.push_back({{"entry3", 10.3, "#3"}});
	entries.push_back({{"entry4", 14.0, "#4"}});
	entries.push_back({{"entry5", 55.0, "#5"}});
	entries.push_back({{"entry6", 66.6, "#6"}});
	for(int i = 0; i < entries.size(); i++){
		t.apiAddEntry("ReadEntry", entries[i]);
	}



	cout << "delete Entry Tests" << endl;

	cout << "\tValidDeleteWholeEntry: ";
	if(true == t.apiRemoveEntry("DeleteEntry", {{"Third  Thing", 0, "#1"}}))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;


	cout << "\tValidDeleteAll: ";
	if(true == t.apiRemoveEntry("DeleteEntry", "String", {}))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	cout << "\tDeleteKeyValue: ";
	if(false == t.apiRemoveEntry("DeleteEntry", "Double", {}))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;


	cout << "\tEntryDoesNotExist: ";
	if(false == t.apiRemoveEntry("DeleteEntry", "", {}))
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;


	//Integration tests
	if(false == integrationTest)
		return;
	cout << "\tDeletedEntry" << endl;
	returned = t.apiReadEntry("DeleteEntry", {"String"});
	if(nullptr == &get<0>(returned[0][0]))	//Deleted column elements return nullptr. I think this allows for that check?
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

}

void readEntryTest(){
	t.apiAddTable("ReadEntry", {{"String", 0}, {"Double", 1}, {"Third Thing", 0}}, {1});
	vector<vector<variant<string, double>>> returned;
	vector<vector<variant<string, double>>> entries;
	entries.push_back({{"entry1", 10.0, "#1"}});
	entries.push_back({{"entry2", 20.0, "#2"}});
	entries.push_back({{"entry3", 10.3, "#3"}});
	entries.push_back({{"entry4", 14.0, "#4"}});
	entries.push_back({{"entry5", 55.0, "#5"}});
	entries.push_back({{"entry6", 66.6, "#6"}});
	for(int i = 0; i < entries.size(); i++){
		t.apiAddEntry("ReadEntry", entries[i]);
	}



	cout << "Read Entry Tests" << endl;
	
	cout << "\tNoCondition: ";
	returned = t.apiReadEntry("ReadEntry", {"String", "Double", "Third Thing"});
	bool success = true;
	for(int i = 0; entries.size(); i++){
		if(get<0>(entries[i][0]) != get<0>(returned[i][0])){
			success = false;
			break;
		}
		else if(get<1>(entries[i][1]) != get<1>(returned[i][1])){
			success = false;
			break;
		}
		else if(get<0>(entries[i][2]) != get<0>(returned[i][2])){
			success = false;
			break;
		}
	}
	if(true == success)
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	cout << "\tOneCondition: ";
	returned = t.apiReadEntry("ReadEntry", {"String", "Double", "Third Thing"}, {{"Double", 1, 10.0}});
	for(int i = 1; entries.size(); i++){
		if(get<0>(entries[i][0]) != get<0>(returned[i-1][0])){
			success = false;
			break;
		}
		else if(get<1>(entries[i][1]) != get<1>(returned[i-1][1])){
			success = false;
			break;
		}
		else if(get<0>(entries[i][2]) != get<0>(returned[i-1][2])){
			success = false;
			break;
		}
	}
	if(true == success)
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	cout << "\tMultipleConditions: ";
	returned = t.apiReadEntry("ReadEntry", {"String", "Double", "Third Thing"}, {{"Double", 1, 10.0}, {"String", 0, "entry3"}});
	for(int i = 1; entries.size(); i++){
		if(get<0>(entries[i][0]) != get<0>(returned[i-1][0])){
			success = false;
			break;
		}
		else if(get<1>(entries[i][1]) != get<1>(returned[i-1][1])){
			success = false;
			break;
		}
		else if(get<0>(entries[i][2]) != get<0>(returned[i-1][2])){
			success = false;
			break;
		}
	}
	if(true == success)
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	cout << "\tNoResults: ";
	returned = t.apiReadEntry("ReadEntry", {"String", "Double", "Third Thing"}, {{"Double", 3, 10.0}, {"String", 0, "entry3"}});
	if(0 == returned.size())
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;
		
	cout << "\tInvalidOperator: ";
	returned = t.apiReadEntry("ReadEntry", {"String", "Double", "Third Thing"}, {{"String", 3, "entry1"}});
	if(0 == returned.size())
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

	cout << "\tWrongType: ";
	returned = t.apiReadEntry("ReadEntry", {"String", "Double", "Third Thing"}, {{"Double", 3, "entry1"}});
	if(0 == returned.size())
		cout << "Success" << endl;
	else
		cout << "Failed" << endl;

}

*/

//Comment out here if a test you aren't working on is throwing errors
int main(){
	addTableTest();
	updateTableTest();
	deleteTableTest();
	readTableTest();
	addEntryTest();
	updateEntryTest();
	//deleteEntryTest();
	//readEntryTest();
	return 0;
}