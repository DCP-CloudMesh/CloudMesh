# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# NO CHECKED-IN PROTOBUF GENCODE
# source: proto/utility.proto
# Protobuf Python Version: 5.28.2
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import runtime_version as _runtime_version
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
_runtime_version.ValidateProtobufRuntimeVersion(
    _runtime_version.Domain.PUBLIC,
    5,
    28,
    2,
    '',
    'proto/utility.proto'
)
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x13proto/utility.proto\x12\x07utility\"%\n\tIpAddress\x12\n\n\x02ip\x18\x01 \x01(\t\x12\x0c\n\x04port\x18\x02 \x01(\x05\"\x97\x01\n\x0c\x41\x64\x64ressTable\x12>\n\raddress_table\x18\x01 \x03(\x0b\x32\'.utility.AddressTable.AddressTableEntry\x1aG\n\x11\x41\x64\x64ressTableEntry\x12\x0b\n\x03key\x18\x01 \x01(\t\x12!\n\x05value\x18\x02 \x01(\x0b\x32\x12.utility.IpAddress:\x02\x38\x01\x62\x06proto3')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'proto.utility_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
  DESCRIPTOR._loaded_options = None
  _globals['_ADDRESSTABLE_ADDRESSTABLEENTRY']._loaded_options = None
  _globals['_ADDRESSTABLE_ADDRESSTABLEENTRY']._serialized_options = b'8\001'
  _globals['_IPADDRESS']._serialized_start=32
  _globals['_IPADDRESS']._serialized_end=69
  _globals['_ADDRESSTABLE']._serialized_start=72
  _globals['_ADDRESSTABLE']._serialized_end=223
  _globals['_ADDRESSTABLE_ADDRESSTABLEENTRY']._serialized_start=152
  _globals['_ADDRESSTABLE_ADDRESSTABLEENTRY']._serialized_end=223
# @@protoc_insertion_point(module_scope)
