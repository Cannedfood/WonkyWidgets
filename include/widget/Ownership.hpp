#pragma once

namespace widget {

class OwnedObject {
	OwnedObject** mLast = nullptr;
	OwnedObject*  mNext = nullptr;
protected:
	virtual void becameOrphan() = 0;
public:
	OwnedObject() {}
	~OwnedObject() { removeFromOwner(); }

	OwnedObject(OwnedObject&& other) { *this = (OwnedObject&&)(other); }
	OwnedObject& operator=(OwnedObject&& other) {
		mLast = other.mLast;
		if(mLast) *mLast = this;
		mNext = other.mNext;
		if(mNext) mNext->mLast = &mNext;
		other.mNext = nullptr;
		other.mLast = nullptr;
		return *this;
	}

	void insertInto(OwnedObject*& list) {
		removeFromOwner();
		mNext = list;
		if(mNext) mNext->mLast = &mNext;
		list  = this;
		mLast = &list;
	}

	void removeFromOwner() {
		if(mLast) *mLast = mNext;
		if(mNext) mNext->mLast = mLast;
		mLast = nullptr;
		mNext = nullptr;
	}

	void makeOrphan() {
		removeFromOwner();
		becameOrphan();
	}
};

class Owner {
private:
	OwnedObject* mOwnerships = nullptr;
public:
	~Owner() {
		clearOwnerships();
	}

	void clearOwnerships() {
		while(mOwnerships) {
			mOwnerships->makeOrphan();
		}
	}

	void transferOwnership(OwnedObject* object) {
		object->insertInto(mOwnerships);
	}
};

} // namespace widget
