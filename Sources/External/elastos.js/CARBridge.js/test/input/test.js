
let assert = require('assert');

let fs = require('fs');


let random = require('../__common__/js/random.js');


let CARBridge = require('elastos.js/CARBridge.js');



let Test = CARBridge.require('CARBridge.js.test.input.Test.eco');

let outputFD = fs.openSync(Test.OUTPUT_FILE, 'r');

let test = new Test.CTest();

console.log('Test "CTest::TestSelf(void)"');

test.TestSelf();

let output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

assert(/\s*0[Xx][0-9A-Fa-f]+\s*\(\s*Interface\s*\(\s*ITest\s*\)\s**\s*\)\s*/.test(output));

console.log('Test "CTest::TestVoid(void)"');

test.TestVoid();

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

assert(/\s*void\s*\(\s*void\s*\)\s*/.test(output));

let i16 = random.int16();

console.log(`Test "CTest::TestInt16(${i16})"`);

test.TestInt16(i16);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

let matches = output.match(/(.*)\(\s*Int16\s*\)\s*/);

assert(matches !== null);

assert(matches[1] == i16);

let i32 = random.int32();

console.log(`Test "CTest::TestInt32(${i32})"`);

test.TestInt32(i32);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Int32\s*\)\s*/);

assert(matches !== null);

assert(matches[1] == i32);

let i64 = random.int64();

console.log(`Test "CTest::TestInt64(${i64})"`);

test.TestInt64(i64);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Int64\s*\)\s*/);

assert(matches !== null);

assert(matches[1] == i64);

let byte_ = random.byte_();

console.log(`Test "CTest::TestByte(${byte_})"`);

test.TestByte(byte_);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Byte\s*\)\s*/);

assert(matches !== null);

assert(matches[1] == byte_);

let f = random.float_();

console.log(`Test "CTest::TestFloat(${f})"`);

test.TestFloat(f);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Float\s*\)\s*/);

assert(matches !== null);

assert(matches[1] == f);

let d = random.double_();

console.log(`Test "CTest::TestDouble(${d})"`);

test.TestDouble(d);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Double\s*\)\s*/);

assert(matches !== null);

assert(matches[1] == d);

let c32 = random.char32();

console.log(`Test "CTest::TestChar32(${c32})"`);

test.TestChar32(c32);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Char32\s*\)\s*/);

assert(matches !== null);

assert(matches[1] == c32);

let s = random.string();

console.log(`Test "CTest::TestString(${s})"`);

test.TestString(s);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*String\s*\)\s*/);

assert(matches !== null);

assert(JSON.parse(matches[1]) === s);

let b = random.boolean_();

console.log(`Test "CTest::TestBoolean(${b})"`);

test.TestBoolean(b);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Boolean\s*\)\s*/);

assert(matches !== null);

assert(JSON.parse(matches[1]) === b);

let emuid = random.emuid();

console.log(`Test "CTest::TestEMuid(${JSON.stringify(emuid)})"`);

test.TestEMuid(emuid);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*EMuid\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), emuid);

let eguid = random.eguid();

console.log(`Test "CTest::TestEGuid(${JSON.stringify(eguid)})"`);

test.TestEGuid(emuid);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*EGuid\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), eguid);

let ecode = random.ecode();

console.log(`Test "CTest::TestECode(${ecode})"`);

test.TestECode(ecode);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ECode\s*\)\s*/);

assert(matches !== null);

assert(matches[1] == ecode);

let localPtr = random.localPtr();

console.log(`Test "CTest::TestLocalPtr(${localPtr})"`);

test.TestLocalPtr(localPtr);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*LocalPtr\s*\(\s*PVoid\s*\)\s*\)\s*/);

assert(matches !== null);

assert(matches[1] == localPtr);

let randomETest = () => {
  return random.pick([etest of Test.ETest]);
};

let etest = randomETest();

console.log(`Test "CTest::TestEnum(${etest.$name})"`);

test.TestEnum(etest);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Enum\s*\(\s*ETest\s*\)\s*\)\s*/);

assert(matches !== null);

assert(JSON.parse(matches[1]) === etest.$name);

let i16s = random.int16s();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(i16s)})"`);

test.TestCARArray(i16s);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*Int16\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), i16s);

let i32s = random.int32s();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(i32s)})"`);

test.TestCARArray(i32s);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*Int32\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), i32s);

let i64s = random.int64s();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(i64s)})"`);

test.TestCARArray(i64s);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*Int64\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), i64s);

let bytes = random.bytes();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(bytes)})"`);

test.TestCARArray(bytes);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*Byte\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), bytes);

let fs_ = random.floats();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(fs_)})"`);

test.TestCARArray(fs_);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*Float\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), fs_);

let ds = random.doubles();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(ds)})"`);

test.TestCARArray(ds);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*Double\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), ds);

let randomETests = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let etests = [];

  for (let i = 0; i < length; ++i)
    etests.push(randomETest());

  return etests;
};

let etests = randomETests();

console.log(`Test "CTest::TestCARArrayOfEnum(${JSON.stringify(etests)})"`);

test.TestCARArrayOfEnum(etests);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*ETest\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), etests);

let c32s = random.char32s();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(c32s)})"`);

test.TestCARArray(c32s);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*Char32\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), c32s);

let ss = random.strings();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(ss)})"`);

test.TestCARArray(ss);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*String\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), ss);

let bs = random.booleans();

console.log(`Test "CTest::TestCARArrayOfBoolean(${JSON.stringify(bs)})"`);

test.TestCARArrayOfBoolean(bs);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*Boolean\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), bs);

let emuids = random.emuids();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(emuids)})"`);

test.TestCARArray(emuids);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*EMuid\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), emuids);

let eguids = random.eguids();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(eguids)})"`);

test.TestCARArray(eguids);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*EGuid\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), eguids);

let ecodes = random.ecodes();

console.log(`Test "CTest::TestCARArrayOfECode(${JSON.stringify(ecodes)})"`);

test.TestCARArrayOfECode(ecodes);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*ECode\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), ecodes);

let randomITest = () => {
  return new Test.CTest();
};

let randomITests = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let itests = [];

  for (let i = 0; i < length; ++i)
    itests.push(randomITest());

  return itests;
};

let itests = randomITests();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(itests)})"`);

test.TestCARArray(itests);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*ITest\s**\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), itests);

let randomSTestHelper = () => {
  return {
    'i16': random.int16()
  };
};

let randomSTestHelpers = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let stests = [];

  for (let i = 0; i < length; ++i)
    stests.push(randomSTestHelper());

  return stests;
};

let randomSTest = () => {
  return {
    'i16': random.int16(),
    'i32': random.int32(),
    'i64': random.int64(),
    'byte': random.byte_(),
    'f': random.float_(),
    'd': random.double_(),
    'c32': random.char32(),
    'b': random.boolean_(),
    'emuid': random.emuid(),
    'eguid': random.eguid(),
    'ecode': random.ecode(),
    'ptr': random.localPtr(),
    'etest': randomETest(),
    'stest': randomSTestHelper(),
    'vi16': random.int16s(2),
    'vi32': random.int32s(2),
    'vi64': random.int64s(2),
    'vbyte': random.bytes(2),
    'vf': random.floats(2),
    'vd': random.doubles(2),
    'vc32': random.char32s(2),
    'vb': random.booleans(2),
    'vemuid': random.emuids(2),
    'veguid': random.eguids(2),
    'vecode': random.ecodes(2),
    'vptr': randomLocalPtrs(2),
    'vetest': randomETests(2),
    'vstest': randomSTestHelpers(2),
    'vvi16': [random.int16s(2), random.int16s(2)],
    'vvi32': [random.int32s(2), random.int32s(2)],
    'vvi64': [random.int64s(2), random.int64s(2)],
    'vvbyte': [random.bytes(2), random.bytes(2)],
    'vvf': [random.floats(2), random.floats(2)],
    'vvd': [random.doubles(2), random.doubles(2)],
    'vvc32': [random.char32s(2), random.char32s(2)],
    'vvb': [random.booleans(2), random.booleans(2)],
    'vvemuid': [random.emuids(2), random.emuids(2)],
    'vveguid': [random.eguids(2), random.eguids(2)],
    'vvecode': [random.ecodes(2), random.ecodes(2)],
    'vvptr': [randomLocalPtrs(2), randomLocalPtrs(2)],
    'vvetest': [randomETests(2), randomETests(2)],
    'vvstest': [randomSTestHelpers(2), randomSTestHelpers(2)]
  };
};

let randomSTests = length => {
  if (typeof length !== 'number' || length < 0)
    length = random.integer(0, UINT32_MAX);

  let stests = [];

  for (let i = 0; i < length; ++i)
    stests.push(randomSTest());

  return stests;
};

let stests = randomSTests();

console.log(`Test "CTest::TestCARArray(${JSON.stringify(stests)})"`);

test.TestCARArray(stests);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*ArrayOf\s*<\s*STest\s*>\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), stests);

let stest = randomSTest();

console.log(`Test "CTest::TestStruct(${JSON.stringify(stest)})"`);

test.TestStruct(stest);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Struct\s*\(\s*STest\s*\)\s*\)\s*/);

assert(matches !== null);

assert.deepStrictEqual(JSON.parse(matches[1]), stest);

let itest = randomITest();

console.log(`Test "CTest::TestInterface(${itest})"`);

test.TestInterface(itest);

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Interface\s*\(\s*ITest\s**\s*\)\s*\)\s*/);

assert(matches !== null);

let _itest = matches[1];

itest.TestSelf();

output = fs.readFileSync(outputFD, 'UTF8');

console.log(output);

matches = output.match(/(.*)\(\s*Interface\s*\(\s*ITest\s**\s*\)\s*\)\s*/);

assert(matches !== null);

let __itest = matches[1];

assert(JSON.parse(_itest) === JSON.parse(__itest));

