import Testing
@testable import CXXAudioToolbox

@Suite struct CXXCoreAudioTests {
	@Test func audioConverter() async {
		var converter = CXXAudioToolbox.ATAudioConverter()
		#expect(converter.IsValid() == false)
	}

	@Test func audioFile() async {
		var af = CXXAudioToolbox.ATAudioFile()
		#expect(af.IsValid() == false)
	}
}
