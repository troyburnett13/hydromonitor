
<template>
    <VContainer align="center" fluid>
        <VRow justify="center" style="max-width: 1200px;">
            <VCol align="center">
                <v-sheet class="mb-1 w-100 rounded-lg" style="max-width: 800;" color="surface">
                    <v-card class="text-secondary" title="LED Controls" color="surface" subtitle="Recent settings" variant="tonal" flat></v-card>
                </v-sheet>
                <v-sheet class="mb-1 w-100" style="max-width: 800;" color="surface">
                    <v-card class="pt-5" color="surface" variant="tonal">
                        <v-slider class="pt-2 bg-surface" append-icon="mdi:mdi-car-light-high" density="compact" thumb-size="16" color="secondary" label="Brightness" min="0" max="250" step="10" show-ticks thumb-label="always" v-model="led.brightness"></v-slider>
                    </v-card>
                </v-sheet>
                <v-sheet class="mb-1 w-100" justify="center" style="max-width: 800;" color="surface">
                    <v-card class="pt-5" color="surface" variant="tonal">
                        <v-slider class="pt-2 bg-surface" append-icon="mdi:mdi-led-on" density="compact" thumb-size="16" color="secondary" label="LED Nodes" min="1" max="7" step="1" show-ticks thumb-label="always" v-model="led.leds"></v-slider>
                    </v-card>
                </v-sheet>
                <v-sheet class="mb-1 w-100 pa-2" justify="center" border style="max-width: 800;" color="surface">
                    <v-progress-circular rotate="0" size="200" :width="15" :model-value="led.leds *15" :color="indicatorColor"><span class="text-onSurface font-weight-bold">{{ led.leds }} LED(s)</span></v-progress-circular>
                </v-sheet>
            </VCol>
            <VCol align="center">
                <v-color-picker v-model="led.color"></v-color-picker>
            </VCol>

        </VRow>
    </VContainer>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { ref,reactive,watch ,onMounted,onBeforeUnmount,computed } from "vue";  
import { useRoute ,useRouter } from "vue-router";
import { useMqttStore } from '@/store/mqttStore'; // Import Mqtt Store
import { storeToRefs } from "pinia";


// VARIABLES
const Mqtt = useMqttStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);
const router      = useRouter();
const route       = useRoute(); 
const led = reactive({"brightness":255,"leds":1,"color":{ r: 255, g: 0, b: 255, a: 1 }});
let timer, ID = 1000; 


// FUNCTIONS
onMounted(()=>{
    // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
    Mqtt.connect(); // Connect to Broker located on the backend
    setTimeout( ()=>{
        // Subscribe to each topic
        Mqtt.subscribe("620141171");
        //Mqtt.subscribe("topic2");
    },3000);
});


onBeforeUnmount(()=>{
    // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
    // unsubscribe from all topics
    Mqtt.unsubcribeAll();
});


// WATCHERS
watch(led,(controls)=>{
    clearTimeout(ID);
    ID = setTimeout(()=>{
        const message = JSON.stringify({"type":"controls","brightness":controls.brightness,"leds":controls.leds,"color":
        controls.color});
        Mqtt.publish("620141171_sub",message); // Publish to a topic subscribed to by the hardware
    },1000)
})

// COMPUTED PROPERTIES
    const indicatorColor = computed(()=>{
        return `rgba(${led.color.r},${led.color.g},${led.color.b},${led.color.a})`
})


</script>


<style scoped>
/** CSS STYLE HERE */


</style>
  
