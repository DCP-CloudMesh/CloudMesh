# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: proto/payload.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf.internal import enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from proto import utility_pb2 as proto_dot_utility__pb2

from proto.utility_pb2 import *

DESCRIPTOR = _descriptor.FileDescriptor(
  name='proto/payload.proto',
  package='payload',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=_b('\n\x13proto/payload.proto\x12\x07payload\x1a\x13proto/utility.proto\"\xc7\x03\n\x0ePayloadMessage\x12\n\n\x02id\x18\x01 \x01(\t\x12\x10\n\x08senderId\x18\x02 \x01(\t\x12+\n\x0fsenderIpAddress\x18\x03 \x01(\x0b\x32\x12.utility.IpAddress\x12)\n\x0bpayloadType\x18\x04 \x01(\x0e\x32\x14.payload.PayloadType\x12\x35\n\x10\x64iscoveryRequest\x18\x06 \x01(\x0b\x32\x19.payload.DiscoveryRequestH\x00\x12\x37\n\x11\x64iscoveryResponse\x18\x07 \x01(\x0b\x32\x1a.payload.DiscoveryResponseH\x00\x12+\n\x0btaskRequest\x18\x08 \x01(\x0b\x32\x14.payload.TaskRequestH\x00\x12-\n\x0ctaskResponse\x18\t \x01(\x0b\x32\x15.payload.TaskResponseH\x00\x12\x33\n\x0f\x61\x63knowledgement\x18\n \x01(\x0b\x32\x18.payload.AcknowledgementH\x00\x12-\n\x0cregistration\x18\x0b \x01(\x0b\x32\x15.payload.RegistrationH\x00\x42\x0f\n\rpayloadObject\"B\n\x11\x44iscoveryResponse\x12-\n\x0e\x61vailablePeers\x18\x01 \x01(\x0b\x32\x15.utility.AddressTable\"*\n\x10\x44iscoveryRequest\x12\x16\n\x0epeersRequested\x18\x01 \x01(\x05\"\xbd\x01\n\x0bTaskRequest\x12\x12\n\nnumWorkers\x18\x01 \x01(\x05\x12\x12\n\nleaderUuid\x18\x02 \x01(\t\x12.\n\x0f\x61ssignedWorkers\x18\x03 \x01(\x0b\x32\x15.utility.AddressTable\x12\x16\n\x0cglob_pattern\x18\x04 \x01(\tH\x00\x12&\n\x1ctraining_data_index_filename\x18\x05 \x01(\tH\x00\x42\x16\n\x14training_data_source\"4\n\x0cTrainingData\x12$\n\x1ctraining_data_index_filename\x18\x01 \x01(\t\"-\n\x13\x41ggregatorInputData\x12\x16\n\x0emodelStateDict\x18\x01 \x01(\x0c\"&\n\x0cTaskResponse\x12\x16\n\x0emodelStateDict\x18\x01 \x01(\x0c\"\x11\n\x0f\x41\x63knowledgement\"\x0e\n\x0cRegistration*\x88\x01\n\x0bPayloadType\x12\x13\n\x0f\x41\x43KNOWLEDGEMENT\x10\x00\x12\x10\n\x0cREGISTRATION\x10\x01\x12\x15\n\x11\x44ISCOVERY_REQUEST\x10\x02\x12\x16\n\x12\x44ISCOVERY_RESPONSE\x10\x03\x12\x10\n\x0cTASK_REQUEST\x10\x04\x12\x11\n\rTASK_RESPONSE\x10\x05P\x00\x62\x06proto3')
  ,
  dependencies=[proto_dot_utility__pb2.DESCRIPTOR,],
  public_dependencies=[proto_dot_utility__pb2.DESCRIPTOR,])

_PAYLOADTYPE = _descriptor.EnumDescriptor(
  name='PayloadType',
  full_name='payload.PayloadType',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='ACKNOWLEDGEMENT', index=0, number=0,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='REGISTRATION', index=1, number=1,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='DISCOVERY_REQUEST', index=2, number=2,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='DISCOVERY_RESPONSE', index=3, number=3,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='TASK_REQUEST', index=4, number=4,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='TASK_RESPONSE', index=5, number=5,
      serialized_options=None,
      type=None),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=992,
  serialized_end=1128,
)
_sym_db.RegisterEnumDescriptor(_PAYLOADTYPE)

PayloadType = enum_type_wrapper.EnumTypeWrapper(_PAYLOADTYPE)
ACKNOWLEDGEMENT = 0
REGISTRATION = 1
DISCOVERY_REQUEST = 2
DISCOVERY_RESPONSE = 3
TASK_REQUEST = 4
TASK_RESPONSE = 5



_PAYLOADMESSAGE = _descriptor.Descriptor(
  name='PayloadMessage',
  full_name='payload.PayloadMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='payload.PayloadMessage.id', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='senderId', full_name='payload.PayloadMessage.senderId', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='senderIpAddress', full_name='payload.PayloadMessage.senderIpAddress', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='payloadType', full_name='payload.PayloadMessage.payloadType', index=3,
      number=4, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='discoveryRequest', full_name='payload.PayloadMessage.discoveryRequest', index=4,
      number=6, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='discoveryResponse', full_name='payload.PayloadMessage.discoveryResponse', index=5,
      number=7, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='taskRequest', full_name='payload.PayloadMessage.taskRequest', index=6,
      number=8, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='taskResponse', full_name='payload.PayloadMessage.taskResponse', index=7,
      number=9, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='acknowledgement', full_name='payload.PayloadMessage.acknowledgement', index=8,
      number=10, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='registration', full_name='payload.PayloadMessage.registration', index=9,
      number=11, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
    _descriptor.OneofDescriptor(
      name='payloadObject', full_name='payload.PayloadMessage.payloadObject',
      index=0, containing_type=None, fields=[]),
  ],
  serialized_start=54,
  serialized_end=509,
)


_DISCOVERYRESPONSE = _descriptor.Descriptor(
  name='DiscoveryResponse',
  full_name='payload.DiscoveryResponse',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='availablePeers', full_name='payload.DiscoveryResponse.availablePeers', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=511,
  serialized_end=577,
)


_DISCOVERYREQUEST = _descriptor.Descriptor(
  name='DiscoveryRequest',
  full_name='payload.DiscoveryRequest',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='peersRequested', full_name='payload.DiscoveryRequest.peersRequested', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=579,
  serialized_end=621,
)


_TASKREQUEST = _descriptor.Descriptor(
  name='TaskRequest',
  full_name='payload.TaskRequest',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='numWorkers', full_name='payload.TaskRequest.numWorkers', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='leaderUuid', full_name='payload.TaskRequest.leaderUuid', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='assignedWorkers', full_name='payload.TaskRequest.assignedWorkers', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='glob_pattern', full_name='payload.TaskRequest.glob_pattern', index=3,
      number=4, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='training_data_index_filename', full_name='payload.TaskRequest.training_data_index_filename', index=4,
      number=5, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
    _descriptor.OneofDescriptor(
      name='training_data_source', full_name='payload.TaskRequest.training_data_source',
      index=0, containing_type=None, fields=[]),
  ],
  serialized_start=624,
  serialized_end=813,
)


_TRAININGDATA = _descriptor.Descriptor(
  name='TrainingData',
  full_name='payload.TrainingData',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='training_data_index_filename', full_name='payload.TrainingData.training_data_index_filename', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=815,
  serialized_end=867,
)


_AGGREGATORINPUTDATA = _descriptor.Descriptor(
  name='AggregatorInputData',
  full_name='payload.AggregatorInputData',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='modelStateDict', full_name='payload.AggregatorInputData.modelStateDict', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=869,
  serialized_end=914,
)


_TASKRESPONSE = _descriptor.Descriptor(
  name='TaskResponse',
  full_name='payload.TaskResponse',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='modelStateDict', full_name='payload.TaskResponse.modelStateDict', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=916,
  serialized_end=954,
)


_ACKNOWLEDGEMENT = _descriptor.Descriptor(
  name='Acknowledgement',
  full_name='payload.Acknowledgement',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=956,
  serialized_end=973,
)


_REGISTRATION = _descriptor.Descriptor(
  name='Registration',
  full_name='payload.Registration',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=975,
  serialized_end=989,
)

_PAYLOADMESSAGE.fields_by_name['senderIpAddress'].message_type = proto_dot_utility__pb2._IPADDRESS
_PAYLOADMESSAGE.fields_by_name['payloadType'].enum_type = _PAYLOADTYPE
_PAYLOADMESSAGE.fields_by_name['discoveryRequest'].message_type = _DISCOVERYREQUEST
_PAYLOADMESSAGE.fields_by_name['discoveryResponse'].message_type = _DISCOVERYRESPONSE
_PAYLOADMESSAGE.fields_by_name['taskRequest'].message_type = _TASKREQUEST
_PAYLOADMESSAGE.fields_by_name['taskResponse'].message_type = _TASKRESPONSE
_PAYLOADMESSAGE.fields_by_name['acknowledgement'].message_type = _ACKNOWLEDGEMENT
_PAYLOADMESSAGE.fields_by_name['registration'].message_type = _REGISTRATION
_PAYLOADMESSAGE.oneofs_by_name['payloadObject'].fields.append(
  _PAYLOADMESSAGE.fields_by_name['discoveryRequest'])
_PAYLOADMESSAGE.fields_by_name['discoveryRequest'].containing_oneof = _PAYLOADMESSAGE.oneofs_by_name['payloadObject']
_PAYLOADMESSAGE.oneofs_by_name['payloadObject'].fields.append(
  _PAYLOADMESSAGE.fields_by_name['discoveryResponse'])
_PAYLOADMESSAGE.fields_by_name['discoveryResponse'].containing_oneof = _PAYLOADMESSAGE.oneofs_by_name['payloadObject']
_PAYLOADMESSAGE.oneofs_by_name['payloadObject'].fields.append(
  _PAYLOADMESSAGE.fields_by_name['taskRequest'])
_PAYLOADMESSAGE.fields_by_name['taskRequest'].containing_oneof = _PAYLOADMESSAGE.oneofs_by_name['payloadObject']
_PAYLOADMESSAGE.oneofs_by_name['payloadObject'].fields.append(
  _PAYLOADMESSAGE.fields_by_name['taskResponse'])
_PAYLOADMESSAGE.fields_by_name['taskResponse'].containing_oneof = _PAYLOADMESSAGE.oneofs_by_name['payloadObject']
_PAYLOADMESSAGE.oneofs_by_name['payloadObject'].fields.append(
  _PAYLOADMESSAGE.fields_by_name['acknowledgement'])
_PAYLOADMESSAGE.fields_by_name['acknowledgement'].containing_oneof = _PAYLOADMESSAGE.oneofs_by_name['payloadObject']
_PAYLOADMESSAGE.oneofs_by_name['payloadObject'].fields.append(
  _PAYLOADMESSAGE.fields_by_name['registration'])
_PAYLOADMESSAGE.fields_by_name['registration'].containing_oneof = _PAYLOADMESSAGE.oneofs_by_name['payloadObject']
_DISCOVERYRESPONSE.fields_by_name['availablePeers'].message_type = proto_dot_utility__pb2._ADDRESSTABLE
_TASKREQUEST.fields_by_name['assignedWorkers'].message_type = proto_dot_utility__pb2._ADDRESSTABLE
_TASKREQUEST.oneofs_by_name['training_data_source'].fields.append(
  _TASKREQUEST.fields_by_name['glob_pattern'])
_TASKREQUEST.fields_by_name['glob_pattern'].containing_oneof = _TASKREQUEST.oneofs_by_name['training_data_source']
_TASKREQUEST.oneofs_by_name['training_data_source'].fields.append(
  _TASKREQUEST.fields_by_name['training_data_index_filename'])
_TASKREQUEST.fields_by_name['training_data_index_filename'].containing_oneof = _TASKREQUEST.oneofs_by_name['training_data_source']
DESCRIPTOR.message_types_by_name['PayloadMessage'] = _PAYLOADMESSAGE
DESCRIPTOR.message_types_by_name['DiscoveryResponse'] = _DISCOVERYRESPONSE
DESCRIPTOR.message_types_by_name['DiscoveryRequest'] = _DISCOVERYREQUEST
DESCRIPTOR.message_types_by_name['TaskRequest'] = _TASKREQUEST
DESCRIPTOR.message_types_by_name['TrainingData'] = _TRAININGDATA
DESCRIPTOR.message_types_by_name['AggregatorInputData'] = _AGGREGATORINPUTDATA
DESCRIPTOR.message_types_by_name['TaskResponse'] = _TASKRESPONSE
DESCRIPTOR.message_types_by_name['Acknowledgement'] = _ACKNOWLEDGEMENT
DESCRIPTOR.message_types_by_name['Registration'] = _REGISTRATION
DESCRIPTOR.enum_types_by_name['PayloadType'] = _PAYLOADTYPE
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

PayloadMessage = _reflection.GeneratedProtocolMessageType('PayloadMessage', (_message.Message,), dict(
  DESCRIPTOR = _PAYLOADMESSAGE,
  __module__ = 'proto.payload_pb2'
  # @@protoc_insertion_point(class_scope:payload.PayloadMessage)
  ))
_sym_db.RegisterMessage(PayloadMessage)

DiscoveryResponse = _reflection.GeneratedProtocolMessageType('DiscoveryResponse', (_message.Message,), dict(
  DESCRIPTOR = _DISCOVERYRESPONSE,
  __module__ = 'proto.payload_pb2'
  # @@protoc_insertion_point(class_scope:payload.DiscoveryResponse)
  ))
_sym_db.RegisterMessage(DiscoveryResponse)

DiscoveryRequest = _reflection.GeneratedProtocolMessageType('DiscoveryRequest', (_message.Message,), dict(
  DESCRIPTOR = _DISCOVERYREQUEST,
  __module__ = 'proto.payload_pb2'
  # @@protoc_insertion_point(class_scope:payload.DiscoveryRequest)
  ))
_sym_db.RegisterMessage(DiscoveryRequest)

TaskRequest = _reflection.GeneratedProtocolMessageType('TaskRequest', (_message.Message,), dict(
  DESCRIPTOR = _TASKREQUEST,
  __module__ = 'proto.payload_pb2'
  # @@protoc_insertion_point(class_scope:payload.TaskRequest)
  ))
_sym_db.RegisterMessage(TaskRequest)

TrainingData = _reflection.GeneratedProtocolMessageType('TrainingData', (_message.Message,), dict(
  DESCRIPTOR = _TRAININGDATA,
  __module__ = 'proto.payload_pb2'
  # @@protoc_insertion_point(class_scope:payload.TrainingData)
  ))
_sym_db.RegisterMessage(TrainingData)

AggregatorInputData = _reflection.GeneratedProtocolMessageType('AggregatorInputData', (_message.Message,), dict(
  DESCRIPTOR = _AGGREGATORINPUTDATA,
  __module__ = 'proto.payload_pb2'
  # @@protoc_insertion_point(class_scope:payload.AggregatorInputData)
  ))
_sym_db.RegisterMessage(AggregatorInputData)

TaskResponse = _reflection.GeneratedProtocolMessageType('TaskResponse', (_message.Message,), dict(
  DESCRIPTOR = _TASKRESPONSE,
  __module__ = 'proto.payload_pb2'
  # @@protoc_insertion_point(class_scope:payload.TaskResponse)
  ))
_sym_db.RegisterMessage(TaskResponse)

Acknowledgement = _reflection.GeneratedProtocolMessageType('Acknowledgement', (_message.Message,), dict(
  DESCRIPTOR = _ACKNOWLEDGEMENT,
  __module__ = 'proto.payload_pb2'
  # @@protoc_insertion_point(class_scope:payload.Acknowledgement)
  ))
_sym_db.RegisterMessage(Acknowledgement)

Registration = _reflection.GeneratedProtocolMessageType('Registration', (_message.Message,), dict(
  DESCRIPTOR = _REGISTRATION,
  __module__ = 'proto.payload_pb2'
  # @@protoc_insertion_point(class_scope:payload.Registration)
  ))
_sym_db.RegisterMessage(Registration)


# @@protoc_insertion_point(module_scope)
