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

#ifndef ORION_RESOURCERTTI_H
#define ORION_RESOURCERTTI_H

#include <QtGlobal>

namespace ORION{

#if defined(Q_OS_WIN)
	typedef unsigned __int64 RTTIType;
#elif defined(Q_OS_OSX)
	typedef unsigned long long RTTIType;
#else
	typedef unsigned int64_t  RTTIType;
#endif

	class  ResourceRTTI {
	public:
		ResourceRTTI (const char* inName, const RTTIType  inTypeFlag, const ResourceRTTI* inBaseRTTI );
		inline const char* getName() const {return name_;}
		inline const RTTIType getTypeFlag() const {return typeFlag_;}
		inline const ResourceRTTI* getBaseRTTI() const {return baseRTTI_;}

	protected:
		const RTTIType typeFlag_;
		const char* name_;
		const ResourceRTTI* baseRTTI_;
	};

#define DeclareRootRTTI(classname) \
public: \
	static const RTTIType allowChildFlags_; \
	static const ResourceRTTI RTTI_; \
	virtual const ResourceRTTI* getRTTI() const {return &RTTI_;} \
	virtual unsigned int objectId() const {return 0;} \
	\
	static bool isExactKindOf(const ResourceRTTI* pkRTTI, const classname* pkObject) \
	{ if (!pkObject) { return false; } return pkObject->isExactKindOf(pkRTTI); } \
	\
	bool isExactKindOf(const ResourceRTTI* pkRTTI) const { return (getRTTI() == pkRTTI); } \
	\
	static bool isKindOf(const ResourceRTTI* pkRTTI, const classname* pkObject) \
	{ if (!pkObject) { return false; } return pkObject->isKindOf(pkRTTI); } \
	\
	bool isKindOf(const ResourceRTTI* pkRTTI) const \
	{ const ResourceRTTI* pkTmp = getRTTI(); \
	while (pkTmp) { if (pkTmp == pkRTTI) { return true; } \
	pkTmp = pkTmp->getBaseRTTI(); } return false; } \
	\
	static classname* dynamicCast(const ResourceRTTI* pkRTTI, const classname* pkObject) \
	{ if (!pkObject) { return NULL; } return pkObject->dynamicCast(pkRTTI); } \
	\
	classname* dynamicCast(const ResourceRTTI* pkRTTI) const { return (isKindOf(pkRTTI) ? (classname*) this : 0 ); }

#define DeclareRTTI() \
public: \
	static const RTTIType allowChildFlags_; \
	static const ResourceRTTI RTTI_; \
	static unsigned int unqiueID_; \
	virtual const ResourceRTTI* getRTTI() const {return &RTTI_;} \
	virtual unsigned int objectId() const {return ++unqiueID_;}

#define ImplementRootRTTI(rootclassname, classtype, allowChilds) \
	const RTTIType rootclassname::allowChildFlags_ = allowChilds;\
	const ResourceRTTI rootclassname::RTTI_(#rootclassname, classtype, 0)

#define ImplementRTTI(classname, classtype, baseclassname, allowChilds) \
	const RTTIType classname::allowChildFlags_ = allowChilds;\
	unsigned int classname::unqiueID_ = 0; \
	const ResourceRTTI classname::RTTI_(#classname, classtype, &baseclassname::RTTI_)

#define UIsExactKindOf(classname, pkObject) \
	classname::isExactKindOf(&classname::RTTI_, pkObject)

#define UIsKindOf(classname, pkObject) \
	classname::isKindOf(&classname::RTTI_, pkObject)

#define UStaticCast(classname, pkObject) \
	((classname*) pkObject)

#define UDynamicCast(classname, pkObject) \
	((classname*) classname::dynamicCast(&classname::RTTI_, pkObject))

}

#endif
