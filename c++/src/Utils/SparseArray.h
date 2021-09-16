#ifndef SPARSE_ARRAY_H
#define SPARSE_ARRAY_H

#include <iostream>
#include <map>
#include <pair>
#include <stdexcept>

struct CmpCoords {
	bool operator()(const std::pair<int, int>& lhs,
		const std::pair<int, int>& rhs);
};

// Less overhead from maps
template<typename T>
class SparseArray {
	typedef std::map<std::pair<int, int> T>::iterator Iter;
public:
	SparseArray() : SparseArray(0, 0) {}
	SparseArray(int _w, int _h) : w(_w), h(_h) {}
	~SparseArray() = default;

	bool inBounds(int c, int r) const {
		return c >= 0 && c < w && r >= 0 && r < h;
	}
	std::pair<int, int> getCoord(int c, int r) const {
		return std::make_pair(r, c);
	}

	void insert(int c, int r, const T& val) {
		if (inBounds(c, r)) { mArray[getCoord(c, r)] = val; }
	}
	void remove(int c, int r) {
		auto it = mArray.find(getCoord(c, r));
		if (it != mArray.end()) { mArray.erase(it); }
	}
	const T& get(int c, int r) {
		auto it = mArray.find(getCoord(c, r));
		if (it == mArray.end()) {
			throw std::out_of_range("SparseArray::get(): invalid coordinates");
		}
		return it->second;
	}

	// Iteration range - [begin, end)
	Iter& begin() { return mArray.begin(); }
	Iter& end() { return mArray.end(); }
	// Iteration range - row = row, col = [colLb, colUb)
	Iter& beginRow(int row, int colLb = 0, int colUb = w) {
		return mArray.upper_bound(std::make_pair(row, colLb - 1));
	}
	Iter& endRow(int row, int colLb = 0, int colUb = w) {
		return mArray.upper_bound(std::make_pair(row, colUb));
	}

private:
	int w, h;
	std::map<std::pair<int, int>, T, CmpCoords> mArray;
};

// More structured
/*
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
*/

#endif