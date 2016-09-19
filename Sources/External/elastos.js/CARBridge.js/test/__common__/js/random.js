
let os = require('os');


let random = require('random-js')();



const UINT8_MAX = 255;

const INT16_MIN = -32767 - 1;
const INT16_MAX = 32767;

const UINT16_MAX = 65535;

const INT32_MIN = -2147483647 - 1;
const INT32_MAX = 2147483647;

const UINT32_MAX = 4294967295;

const INT64_MIN = -9223372036854775807 - 1;
const INT64_MAX = 9223372036854775807;

const WORD_SIZE = (() => {
  switch (os.arch()) {
  case 'arm':
    return 32;
  case 'arm64':
    return 64;
  case 'ia32':
    return 32;
  case 'mips':
    return 32;
  case 'mipsel':
    return 64;
  case 'ppc':
    return 32;
  case 'ppc64':
    return 64;
  case 's390':
    return 32;
  case 's390x':
    return 64;
  case 'x32':
    return 32;
  case 'x64':
    return 64;
  case 'x86':
    return 32;
  default:
    break;
  }
})();

const UINTPTR_MAX = (() => {
  if (WORD_SIZE === 64)
    return 18446744073709551615;

  if (WORD_SIZE === 32)
    return 4294967295;
})();

let int16 = () => random.integer(INT16_MIN, INT16_MAX);

let int32 = () => random.integer(INT32_MIN, INT32_MAX);

let int64 = () => random.integer(INT64_MIN, INT64_MAX);

let byte_ = () => random.integer(0, UINT8_MAX);

let float_ = () => random.real(-Infinity, Infinity);

let double_ = () => random.real(-Infinity, Infinity);

let char32 = () => random.string(1);

let string = () => random.string(256);

let boolean_ = () => random.bool();

let uint32 = () => random.integer(0, UINT32_MAX);

let emuid = () => {
  'mData1': uint32(),
  'mData2': random.integer(0, UINT16_MAX),
  'mData3': random.integer(0, UINT16_MAX),
  'mData4': [
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX),
    random.integer(0, UINT8_MAX)
  ]
};

let eguid = () => {
  'mClsid': emuid(),
  'mUunm': string(),
  'mCarcode': uint32()
};

let ecode = int32;

let localPtr = () => random.integer(0, UINTPTR_MAX);

let int16s = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let i16s = [];

  for (let i = 0; i < length; ++i)
    i16s.push(int16());

  return i16s;
};

let int32s = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let i32s = [];

  for (let i = 0; i < length; ++i)
    i32s.push(int32());

  return i32s;
};

let int64s = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let i64s = [];

  for (let i = 0; i < length; ++i)
    i64s.push(int64());

  return i64s;
};

let bytes = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let bytes = [];

  for (let i = 0; i < length; ++i)
    bytes.push(byte_());

  return bytes;
};

let floats = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let fs_ = [];

  for (let i = 0; i < length; ++i)
    fs_.push(float_());

  return fs_;
};

let doubles = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let ds = [];

  for (let i = 0; i < length; ++i)
    ds.push(double_());

  return ds;
};

let char32s = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let c32s = [];

  for (let i = 0; i < length; ++i)
    c32s.push(char32());

  return c32s;
};

let strings = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let ss = [];

  for (let i = 0; i < length; ++i)
    ss.push(string());

  return ss;
};

let booleans = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let bs = [];

  for (let i = 0; i < length; ++i)
    bs.push(random.bool());

  return bs;
};

let emuids = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let emuids = [];

  for (let i = 0; i < length; ++i)
    emuids.push(emuid());

  return emuids;
};

let eguids = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let eguids = [];

  for (let i = 0; i < length; ++i)
    eguids.push(eguid());

  return eguids;
};

let ecodes = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let ecodes = [];

  for (let i = 0; i < length; ++i)
    ecodes.push(ecode());

  return ecodes;
};

exports.int16 = int16;
exports.int32 = int32;
exports.int64 = int64;
exports.byte_ = byte_;
exports.float_ = float_;
exports.double_ = double_;
exports.char32 = char32;
exports.string = string;
exports.boolean_ = boolean_;
exports.uint32 = uint32;
exports.emuid = emuid;
exports.eguid = eguid;
exports.ecode = ecode;
exports.localPtr = localPtr;
exports.int16s = int16s;
exports.int32s = int32s;
exports.int64s = int64s;
exports.bytes = bytes;
exports.floats = floats;
exports.doubles = doubles;
exports.char32s = char32s;
exports.strings = strings;
exports.booleans = booleans;
exports.emuids = emuids;
exports.eguids = eguids;
exports.ecodes = ecodes;

