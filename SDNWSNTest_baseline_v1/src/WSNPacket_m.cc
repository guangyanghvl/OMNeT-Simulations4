//
// Generated file, do not edit! Created by opp_msgtool 6.1 from WSNPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "WSNPacket_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(WSNPacket)

WSNPacket::WSNPacket(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

WSNPacket::WSNPacket(const WSNPacket& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

WSNPacket::~WSNPacket()
{
}

WSNPacket& WSNPacket::operator=(const WSNPacket& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void WSNPacket::copy(const WSNPacket& other)
{
    this->srcId = other.srcId;
    this->lastHopId = other.lastHopId;
    this->finalDstId = other.finalDstId;
    this->seq = other.seq;
    this->hopCount = other.hopCount;
    this->ttl = other.ttl;
    this->createdAt = other.createdAt;
}

void WSNPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->srcId);
    doParsimPacking(b,this->lastHopId);
    doParsimPacking(b,this->finalDstId);
    doParsimPacking(b,this->seq);
    doParsimPacking(b,this->hopCount);
    doParsimPacking(b,this->ttl);
    doParsimPacking(b,this->createdAt);
}

void WSNPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->srcId);
    doParsimUnpacking(b,this->lastHopId);
    doParsimUnpacking(b,this->finalDstId);
    doParsimUnpacking(b,this->seq);
    doParsimUnpacking(b,this->hopCount);
    doParsimUnpacking(b,this->ttl);
    doParsimUnpacking(b,this->createdAt);
}

int WSNPacket::getSrcId() const
{
    return this->srcId;
}

void WSNPacket::setSrcId(int srcId)
{
    this->srcId = srcId;
}

int WSNPacket::getLastHopId() const
{
    return this->lastHopId;
}

void WSNPacket::setLastHopId(int lastHopId)
{
    this->lastHopId = lastHopId;
}

int WSNPacket::getFinalDstId() const
{
    return this->finalDstId;
}

void WSNPacket::setFinalDstId(int finalDstId)
{
    this->finalDstId = finalDstId;
}

int WSNPacket::getSeq() const
{
    return this->seq;
}

void WSNPacket::setSeq(int seq)
{
    this->seq = seq;
}

int WSNPacket::getHopCount() const
{
    return this->hopCount;
}

void WSNPacket::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

int WSNPacket::getTtl() const
{
    return this->ttl;
}

void WSNPacket::setTtl(int ttl)
{
    this->ttl = ttl;
}

omnetpp::simtime_t WSNPacket::getCreatedAt() const
{
    return this->createdAt;
}

void WSNPacket::setCreatedAt(omnetpp::simtime_t createdAt)
{
    this->createdAt = createdAt;
}

class WSNPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_srcId,
        FIELD_lastHopId,
        FIELD_finalDstId,
        FIELD_seq,
        FIELD_hopCount,
        FIELD_ttl,
        FIELD_createdAt,
    };
  public:
    WSNPacketDescriptor();
    virtual ~WSNPacketDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(WSNPacketDescriptor)

WSNPacketDescriptor::WSNPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(WSNPacket)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

WSNPacketDescriptor::~WSNPacketDescriptor()
{
    delete[] propertyNames;
}

bool WSNPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<WSNPacket *>(obj)!=nullptr;
}

const char **WSNPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *WSNPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int WSNPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 7+base->getFieldCount() : 7;
}

unsigned int WSNPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_srcId
        FD_ISEDITABLE,    // FIELD_lastHopId
        FD_ISEDITABLE,    // FIELD_finalDstId
        FD_ISEDITABLE,    // FIELD_seq
        FD_ISEDITABLE,    // FIELD_hopCount
        FD_ISEDITABLE,    // FIELD_ttl
        FD_ISEDITABLE,    // FIELD_createdAt
    };
    return (field >= 0 && field < 7) ? fieldTypeFlags[field] : 0;
}

const char *WSNPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "srcId",
        "lastHopId",
        "finalDstId",
        "seq",
        "hopCount",
        "ttl",
        "createdAt",
    };
    return (field >= 0 && field < 7) ? fieldNames[field] : nullptr;
}

int WSNPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "srcId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "lastHopId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "finalDstId") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "seq") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "hopCount") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "ttl") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "createdAt") == 0) return baseIndex + 6;
    return base ? base->findField(fieldName) : -1;
}

const char *WSNPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_srcId
        "int",    // FIELD_lastHopId
        "int",    // FIELD_finalDstId
        "int",    // FIELD_seq
        "int",    // FIELD_hopCount
        "int",    // FIELD_ttl
        "omnetpp::simtime_t",    // FIELD_createdAt
    };
    return (field >= 0 && field < 7) ? fieldTypeStrings[field] : nullptr;
}

const char **WSNPacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *WSNPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int WSNPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    WSNPacket *pp = omnetpp::fromAnyPtr<WSNPacket>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void WSNPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    WSNPacket *pp = omnetpp::fromAnyPtr<WSNPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'WSNPacket'", field);
    }
}

const char *WSNPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    WSNPacket *pp = omnetpp::fromAnyPtr<WSNPacket>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string WSNPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    WSNPacket *pp = omnetpp::fromAnyPtr<WSNPacket>(object); (void)pp;
    switch (field) {
        case FIELD_srcId: return long2string(pp->getSrcId());
        case FIELD_lastHopId: return long2string(pp->getLastHopId());
        case FIELD_finalDstId: return long2string(pp->getFinalDstId());
        case FIELD_seq: return long2string(pp->getSeq());
        case FIELD_hopCount: return long2string(pp->getHopCount());
        case FIELD_ttl: return long2string(pp->getTtl());
        case FIELD_createdAt: return simtime2string(pp->getCreatedAt());
        default: return "";
    }
}

void WSNPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    WSNPacket *pp = omnetpp::fromAnyPtr<WSNPacket>(object); (void)pp;
    switch (field) {
        case FIELD_srcId: pp->setSrcId(string2long(value)); break;
        case FIELD_lastHopId: pp->setLastHopId(string2long(value)); break;
        case FIELD_finalDstId: pp->setFinalDstId(string2long(value)); break;
        case FIELD_seq: pp->setSeq(string2long(value)); break;
        case FIELD_hopCount: pp->setHopCount(string2long(value)); break;
        case FIELD_ttl: pp->setTtl(string2long(value)); break;
        case FIELD_createdAt: pp->setCreatedAt(string2simtime(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'WSNPacket'", field);
    }
}

omnetpp::cValue WSNPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    WSNPacket *pp = omnetpp::fromAnyPtr<WSNPacket>(object); (void)pp;
    switch (field) {
        case FIELD_srcId: return pp->getSrcId();
        case FIELD_lastHopId: return pp->getLastHopId();
        case FIELD_finalDstId: return pp->getFinalDstId();
        case FIELD_seq: return pp->getSeq();
        case FIELD_hopCount: return pp->getHopCount();
        case FIELD_ttl: return pp->getTtl();
        case FIELD_createdAt: return pp->getCreatedAt().dbl();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'WSNPacket' as cValue -- field index out of range?", field);
    }
}

void WSNPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    WSNPacket *pp = omnetpp::fromAnyPtr<WSNPacket>(object); (void)pp;
    switch (field) {
        case FIELD_srcId: pp->setSrcId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_lastHopId: pp->setLastHopId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_finalDstId: pp->setFinalDstId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_seq: pp->setSeq(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_hopCount: pp->setHopCount(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_ttl: pp->setTtl(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_createdAt: pp->setCreatedAt(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'WSNPacket'", field);
    }
}

const char *WSNPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr WSNPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    WSNPacket *pp = omnetpp::fromAnyPtr<WSNPacket>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void WSNPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    WSNPacket *pp = omnetpp::fromAnyPtr<WSNPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'WSNPacket'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

