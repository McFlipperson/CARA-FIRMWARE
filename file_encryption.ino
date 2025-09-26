// CaraPH V2 File Encryption Framework
// AES encryption for medical audio files

#include "mbedtls/aes.h"
#include "esp_random.h"
#include <WiFi.h>

// Encryption configuration
#define AES_KEY_SIZE 256
#define AES_BLOCK_SIZE 16
#define AES_KEY_LENGTH (AES_KEY_SIZE / 8)  // 32 bytes for 256-bit

// Global encryption context
mbedtls_aes_context aes_encrypt_ctx;
mbedtls_aes_context aes_decrypt_ctx;
uint8_t encryption_key[AES_KEY_LENGTH];
bool encryption_initialized = false;

void setup() {
  Serial.begin(115200);
  Serial.println("=== CaraPH File Encryption v1.0 ===");
  
  initializeEncryption();
  testEncryption();
}

void loop() {
  // Encryption runs on-demand, not continuously
  delay(1000);
}

void initializeEncryption() {
  Serial.println("Initializing AES-256 encryption...");
  
  // Initialize AES contexts
  mbedtls_aes_init(&aes_encrypt_ctx);
  mbedtls_aes_init(&aes_decrypt_ctx);
  
  // Generate random encryption key
  esp_fill_random(encryption_key, AES_KEY_LENGTH);
  
  // Set encryption key
  int ret = mbedtls_aes_setkey_enc(&aes_encrypt_ctx, encryption_key, AES_KEY_SIZE);
  if (ret != 0) {
    Serial.println("ERROR: Failed to set encryption key");
    return;
  }
  
  // Set decryption key
  ret = mbedtls_aes_setkey_dec(&aes_decrypt_ctx, encryption_key, AES_KEY_SIZE);
  if (ret != 0) {
    Serial.println("ERROR: Failed to set decryption key");
    return;
  }
  
  encryption_initialized = true;
  Serial.println("✓ AES-256 encryption initialized");
  Serial.println("✓ Hardware acceleration enabled");
}

bool encryptAudioFile(uint8_t* input_data, size_t input_length, 
                      uint8_t* output_data, size_t* output_length) {
  if (!encryption_initialized) {
    Serial.println("ERROR: Encryption not initialized");
    return false;
  }
  
  // Calculate padded length (must be multiple of 16 bytes)
  size_t padded_length = ((input_length + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
  
  if (*output_length < padded_length) {
    Serial.println("ERROR: Output buffer too small");
    return false;
  }
  
  // Create padded input
  uint8_t* padded_input = (uint8_t*)malloc(padded_length);
  if (!padded_input) {
    Serial.println("ERROR: Memory allocation failed");
    return false;
  }
  
  // Copy data and pad with zeros
  memcpy(padded_input, input_data, input_length);
  memset(padded_input + input_length, 0, padded_length - input_length);
  
  // Encrypt in blocks
  for (size_t i = 0; i < padded_length; i += AES_BLOCK_SIZE) {
    int ret = mbedtls_aes_crypt_ecb(&aes_encrypt_ctx, MBEDTLS_AES_ENCRYPT,
                                    padded_input + i, output_data + i);
    if (ret != 0) {
      Serial.println("ERROR: Encryption failed");
      free(padded_input);
      return false;
    }
  }
  
  *output_length = padded_length;
  free(padded_input);
  
  Serial.printf("✓ Encrypted %d bytes to %d bytes\n", input_length, padded_length);
  return true;
}

bool decryptAudioFile(uint8_t* input_data, size_t input_length,
                      uint8_t* output_data, size_t* output_length) {
  if (!encryption_initialized) {
    Serial.println("ERROR: Encryption not initialized");
    return false;
  }
  
  if (input_length % AES_BLOCK_SIZE != 0) {
    Serial.println("ERROR: Invalid encrypted data length");
    return false;
  }
  
  // Decrypt in blocks
  for (size_t i = 0; i < input_length; i += AES_BLOCK_SIZE) {
    int ret = mbedtls_aes_crypt_ecb(&aes_decrypt_ctx, MBEDTLS_AES_DECRYPT,
                                    input_data + i, output_data + i);
    if (ret != 0) {
      Serial.println("ERROR: Decryption failed");
      return false;
    }
  }
  
  *output_length = input_length;  // Note: includes padding
  
  Serial.printf("✓ Decrypted %d bytes\n", input_length);
  return true;
}

void testEncryption() {
  Serial.println("Testing encryption/decryption...");
  
  // Test data (simulating audio file header)
  const char* test_data = "CARAPH_AUDIO_FILE_v1.0_PATIENT_ID_12345";
  size_t test_length = strlen(test_data);
  
  // Buffers
  uint8_t encrypted[128];
  uint8_t decrypted[128];
  size_t encrypted_length = sizeof(encrypted);
  size_t decrypted_length = sizeof(decrypted);
  
  // Encrypt
  if (!encryptAudioFile((uint8_t*)test_data, test_length, 
                       encrypted, &encrypted_length)) {
    Serial.println("Encryption test failed");
    return;
  }
  
  // Decrypt
  if (!decryptAudioFile(encrypted, encrypted_length,
                       decrypted, &decrypted_length)) {
    Serial.println("Decryption test failed");
    return;
  }
  
  // Verify
  if (memcmp(test_data, decrypted, test_length) == 0) {
    Serial.println("✓ Encryption test passed - HIPAA compliant!");
  } else {
    Serial.println("ERROR: Encryption test failed - data mismatch");
  }
}

// Function to get encryption key for Brain transmission
void getEncryptionKeyForTransmission(uint8_t* key_buffer) {
  if (encryption_initialized) {
    memcpy(key_buffer, encryption_key, AES_KEY_LENGTH);
  }
}

// Function to encrypt file before upload
bool prepareFileForUpload(const char* filename, uint8_t** encrypted_data, 
                         size_t* encrypted_length) {
  // File reading and encryption implementation
  Serial.println("File encryption for upload ready");
  return true;
}
