#ifndef SPARSE_ARRAY_H
#define SPARSE_ARRAY_H

#include <iostream>
#include <pair>
#include <map>
#include <list>

// Good for single element access
namespace singleAccess {
	template<typename T>
	class SparseArray {
		typedef std::map<std::pair<int, int> T>::iterator Iter;
	public:
		SparseArray() : SparseArray(0, 0) {}
		SparseArray(int _w, int _h) : w(_w), h(_h) {}
		~SparseArray() = default;

		// Iteration range - [begin, end)
		Iter& begin() { return mArray.begin(); }
		Iter& end() { return mArray.end(); }
		// Iteration range - row = row, col = [colLb, colUb)
		Iter& beginRow(int row, int colLb = 0, int colUb = w);
		Iter& endRow(int row, int colLb = 0, int colUb = w);

	private:
		int w, h;
		std::map<std::pair<int, int>, T> mArray;
	};

	template<typename T>
	inline SparseArray<T>::Iter& SparseArray<T>::beginRow(int row, int colLb, int colUb) {
		return mArray.upper_bound(std::make_pair(row, colLb - 1));
	}
	template<typename T>
	intline SparseArray<T>::Iter& SparseArray<T>::endRow(int row, int colLb, int colUb) {
		return mArray.upper_bound(std::make_pair(row, colUb));
	}
}


// Good for block element access
template<typename T>
class SparseArray {
	typedef std::map<int, T> Row;
	typedef std::map<int, Row> Array;
	typedef Array::iterator RowIter;
	typedef Row::iterator ColIter;
public:
	SparseArray() : SparseArray(0, 0) {}
	SparseArray(int _w, int _h) : w(_w), h(_h) {}
	~SparseArray() = default;

	// Begin/end iterators
	RowIter& begin() { return mArray.begin(); }
	RowIter& end() { return mArray.end(); }
	ColIter& begin(RowIter& r) { return r->second.begin(); }
	ColIter& end(RowIter& r) { return r->second.end(); }
	// Find
	RowIter& find(int row) { return mArray.find(row); }
	ColIter& find(RowIter& row, int col) { return row->second.find(col); }
	// At
	Row& operator[](int row) { return mArray[row]; }
	// Remove
	void remove(int row, int col);

private:
	int w, h;
	Array mArray;
};

template<typename T>
inline void SparseArray<T>::remove(int row, int col) {
	auto row = find(row);
	if (row != end()) { row.erase(col); }
}

#endif