#include "ze/collection/sparse_set.hpp"

using namespace ze;

int main(){
	collection::SparseSet sparse_set;
	sparse_set.Insert(5);
	sparse_set.Size();
	sparse_set.HasElement(5);
	sparse_set.HasElement(2);
	sparse_set.Remove(2);
	sparse_set.Size();
	sparse_set.Remove(5);
	sparse_set.Size();
	sparse_set.Insert(1);
	sparse_set.Insert(29);
	sparse_set.Insert(99);
	sparse_set.Remove(5);
	sparse_set.Remove(29);
	sparse_set.Insert(66);
}