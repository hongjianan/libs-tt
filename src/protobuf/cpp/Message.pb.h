// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Message.proto

#ifndef PROTOBUF_Message_2eproto__INCLUDED
#define PROTOBUF_Message_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3003000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3003000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
namespace message {
class Header;
class HeaderDefaultTypeInternal;
extern HeaderDefaultTypeInternal _Header_default_instance_;
}  // namespace message

namespace message {

namespace protobuf_Message_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static void InitDefaultsImpl();
  static void Shutdown();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_Message_2eproto

// ===================================================================

class Header : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:message.Header) */ {
 public:
  Header();
  virtual ~Header();

  Header(const Header& from);

  inline Header& operator=(const Header& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Header& default_instance();

  static inline const Header* internal_default_instance() {
    return reinterpret_cast<const Header*>(
               &_Header_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(Header* other);

  // implements Message ----------------------------------------------

  inline Header* New() const PROTOBUF_FINAL { return New(NULL); }

  Header* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const Header& from);
  void MergeFrom(const Header& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(Header* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional uint32 length = 1;
  bool has_length() const;
  void clear_length();
  static const int kLengthFieldNumber = 1;
  ::google::protobuf::uint32 length() const;
  void set_length(::google::protobuf::uint32 value);

  // optional uint32 uri_type = 2;
  bool has_uri_type() const;
  void clear_uri_type();
  static const int kUriTypeFieldNumber = 2;
  ::google::protobuf::uint32 uri_type() const;
  void set_uri_type(::google::protobuf::uint32 value);

  // optional uint32 ret_code = 3;
  bool has_ret_code() const;
  void clear_ret_code();
  static const int kRetCodeFieldNumber = 3;
  ::google::protobuf::uint32 ret_code() const;
  void set_ret_code(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:message.Header)
 private:
  void set_has_length();
  void clear_has_length();
  void set_has_uri_type();
  void clear_has_uri_type();
  void set_has_ret_code();
  void clear_has_ret_code();

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::uint32 length_;
  ::google::protobuf::uint32 uri_type_;
  ::google::protobuf::uint32 ret_code_;
  friend struct protobuf_Message_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// Header

// optional uint32 length = 1;
inline bool Header::has_length() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Header::set_has_length() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Header::clear_has_length() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Header::clear_length() {
  length_ = 0u;
  clear_has_length();
}
inline ::google::protobuf::uint32 Header::length() const {
  // @@protoc_insertion_point(field_get:message.Header.length)
  return length_;
}
inline void Header::set_length(::google::protobuf::uint32 value) {
  set_has_length();
  length_ = value;
  // @@protoc_insertion_point(field_set:message.Header.length)
}

// optional uint32 uri_type = 2;
inline bool Header::has_uri_type() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Header::set_has_uri_type() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Header::clear_has_uri_type() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Header::clear_uri_type() {
  uri_type_ = 0u;
  clear_has_uri_type();
}
inline ::google::protobuf::uint32 Header::uri_type() const {
  // @@protoc_insertion_point(field_get:message.Header.uri_type)
  return uri_type_;
}
inline void Header::set_uri_type(::google::protobuf::uint32 value) {
  set_has_uri_type();
  uri_type_ = value;
  // @@protoc_insertion_point(field_set:message.Header.uri_type)
}

// optional uint32 ret_code = 3;
inline bool Header::has_ret_code() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Header::set_has_ret_code() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Header::clear_has_ret_code() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Header::clear_ret_code() {
  ret_code_ = 0u;
  clear_has_ret_code();
}
inline ::google::protobuf::uint32 Header::ret_code() const {
  // @@protoc_insertion_point(field_get:message.Header.ret_code)
  return ret_code_;
}
inline void Header::set_ret_code(::google::protobuf::uint32 value) {
  set_has_ret_code();
  ret_code_ = value;
  // @@protoc_insertion_point(field_set:message.Header.ret_code)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)


}  // namespace message

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Message_2eproto__INCLUDED
