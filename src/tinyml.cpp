#include "tinyml.h"
#include "shared_data.h"


namespace {
    tflite::ErrorReporter *error_reporter = nullptr;
    const tflite::Model *model = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;
    constexpr int kTensorArenaSize = 8 * 1024;
    uint8_t tensor_arena[kTensorArenaSize];
}

void setupTinyML() {
    Serial.println("TensorFlow Lite Init....");
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    model = tflite::GetModel(dht_anomaly_model_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        error_reporter->Report("Lỗi phiên bản mô hình!");
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    if (interpreter->AllocateTensors() != kTfLiteOk) {
        error_reporter->Report("AllocateTensors() failed");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);
    Serial.println("TensorFlow Lite Micro initialized on ESP32.");
}


void tiny_ml_task(void *pvParameters) {
    setupTinyML();

    while (1) {
        // 1. Lấy dữ liệu thực tế từ module shared_data
        float current_temp = get_temperature();
        float current_humi = get_humidity();

        // 2. Đưa dữ liệu vào bộ não AI
        input->data.f[0] = current_temp;
        input->data.f[1] = current_humi;

        // 3. Chạy suy luận (Inference)
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk) {
            Serial.println("AI Invoke failed!");
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        // 4. Đọc kết quả
        // AI sẽ trả về 2 xác suất cho 2 nhãn (0: Bình thường, 1: Bất thường)
        float prob_normal = output->data.f[0];   
        float prob_abnormal = output->data.f[1]; 

        // 5. In ra Serial Monitor cực kỳ dễ hiểu
        Serial.print("[TinyML] Nhiệt độ: "); Serial.print(current_temp);
        Serial.print("C | Độ ẩm: "); Serial.print(current_humi);
        Serial.print("% ===> KẾT LUẬN AI: ");

        if (prob_normal > prob_abnormal) {
            Serial.println("DỮ LIỆU BÌNH THƯỜNG (Tin cậy) ✅");
        } else {
            Serial.println("DỮ LIỆU BẤT THƯỜNG / LỖI ❌");
        }

        // Chạy AI mỗi 2 giây 1 lần
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}