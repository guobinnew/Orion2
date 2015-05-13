/************************************************************************
**
**  Copyright (C) 2015  guobin  <guobin.gb@alibaba-inc.com>
**
**  This file is part of Orion2.
**
**  Orion2 is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation.
**
**  Orion is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Orion.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#ifndef ORION_RESOURCEDATA_H
#define ORION_RESOURCEDATA_H

#include <QMap>
#include <QByteArray>
#include <QDataStream>

namespace ORION{

#ifndef DEF_RESOURCEHASH
   #define DEF_RESOURCEHASH
	typedef unsigned int ResourceHash;  // ×ÊÔ´¾ä±ú
#endif

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	struct ResourceData
	{
		QByteArray toByteArray(){
			QByteArray d;
			QDataStream stream(&d, QIODevice::WriteOnly);
			save(stream);
			return d;
		}

		void fromByteArray(QByteArray& data){
			QDataStream stream(data);
			load(stream);
		}

		virtual void save(QDataStream& stream) = 0;
		virtual void load(QDataStream& stream) = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	template<class T>
	class TResourceMap{
	public:
		TResourceMap(){}
		~TResourceMap(){
			clear();
		}

		T* value(const QString& name, bool insert = false){
			ResourceHash key = hashString(name);
			return value(key, insert);
		}

		T* value(ResourceHash key, bool insert = false){
			T* var = NULL;
			if (container_.contains(key)){
				var = container_[key];
			}
			else if (insert){
				var = new T();
				container_.insert(key, var);
			}
			return var;
		}

		int size() const {
			return container_.size();
		}

		QByteArray toByteArray(){
			QByteArray data;
			{
				QDataStream stream(&data, QIODevice::WriteOnly);
				stream << size();
				QMapIterator<ResourceHash, T*> iter(container_);
				while (iter.hasNext()){
					iter.next();
					stream << iter.key();
					stream << iter.value()->toByteArray();
				}
			}
			return data;
		}

		void fromByteArray(QByteArray& data){
			clear();

			QDataStream steam(data);
			int num;
			steam >> num;

			ResourceHash key;
			QByteArray v;
			for (int i = 0; i < num; i++){
				steam >> key >> v;
				T* t = new T();
				t->fromByteArray(v);
				insert(key, t);
			}
		}

		void clear(){
			foreach(T* t, container_.values())
			{
				if (t){
					delete t;
				}
			}
			container_.clear();
		}

		void insert(ResourceHash key, T* t){
			T* old = value(key);
			if (old && old != t){
				delete old;
				container_[key] = t;
			}
			else if (old == NULL){
				container_.insert(key, t);
			}
		}

		T* take(ResourceHash key){
			T* t = value(key);
			if (t){
				container_.remove(key);
			}
			return t;
		}

		void remove(const QString& name){
			ResourceHash key = hashString(name);
			remove(key);
		}

		void remove(ResourceHash key){
			T* t = take(key);
			if (t){
				delete t;
			}
		}

		QList<T*> values(){
			return container_.values();
		}

		QList<ResourceHash> keys(){
			return container_.keys();
		}

	protected:
		QMap<ResourceHash, T*> container_;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//////////////////////////////////////////////////////////////////////////

	template<class T>
	class TResourceList{
	public:
		TResourceList(){}
		~TResourceList(){
			clear();
		}

		T* add(){
			T* t = new T();
			container_.append(t);
			return t;
		}

		void append( const T*& t){
			container_.append(t);
		}

		T* at(int index){
			if (index < 0 || index >= container_.size()){
				return NULL;
			}
			return container_.at(index);
		}

		void insert(int index,  T* t ){
			if (index < 0 || index > container_.size()){
				index = container_.size();
			}
			container_.insert(index, t);
		}

		int size() const {
			return container_.size();
		}

		QByteArray toByteArray(){
			QByteArray data;
			{
				QDataStream stream(&data, QIODevice::WriteOnly);
				stream << size();
				foreach(T* t, container_){
					stream << t->toByteArray();
				}
			}
			return data;
		}

		void fromByteArray(QByteArray& data){
			clear();

			QDataStream steam(data);
			int num;
			steam >> num;

			QByteArray v;
			for (int i = 0; i < num; i++){
				steam >> v;
				T* t = new T();
				t->fromByteArray(v);
				container_.append(t);
			}
		}

		void clear(){
			foreach(T* t, container_)
			{
				if (t){
					delete t;
				}
			}
			container_.clear();
		}

		void remove( T* t){
			container_.removeOne(t);
		}

		T* take(int index ){
			if (index < 0 || index >= container_.size()){
				return NULL;
			}
			return container_.take(index);
		}

		QList<T*> values(){
			return container_;
		}

	protected:
		QList<T*> container_;
	};


}


#endif
