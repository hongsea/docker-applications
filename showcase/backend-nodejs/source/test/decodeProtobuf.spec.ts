/**
 * tests for decodeProtoBuf
 *
 * 2016-10-11 Ab Reitsma
 */

require("./_logSettings");
import * as Chai from "chai";
var expect = Chai.expect;

import decodeProtobuf from "../code/decodeProtobuf";

describe("Test decodeProtobuf", function () {
    it("should decode a correctly encoded TTN message", function () {
        var testMsgBase64 = "CgQIARgC";
        var testMsgBuffer = new Buffer(testMsgBase64, "base64");
        var result = decodeProtobuf(testMsgBuffer);

        var expectedResult = [{
            "id": 1,
            "error": 0,
            "value1": 1,
            "value2": 0,
            "value3": 0,
            "value4": 0,
            "value5": 0,
            "value6": 0,
            "value7": 0,
            "value8": 0,
            "value9": 0,
            "value10": 0
        }];
        expect(result).to.deep.equal(expectedResult);
    });

    it("should decode a correctly encoded KPN message", function () {
        var testMsgHex = "0a0408011802";
        var testMsgBuffer = new Buffer(testMsgHex, "hex");
        var result = decodeProtobuf(testMsgBuffer);
        var expectedResult = [{
            "id": 1,
            "error": 0,
            "value1": 1,
            "value2": 0,
            "value3": 0,
            "value4": 0,
            "value5": 0,
            "value6": 0,
            "value7": 0,
            "value8": 0,
            "value9": 0,
            "value10": 0
        }];
        expect(result).to.deep.equal(expectedResult);
    });
});
