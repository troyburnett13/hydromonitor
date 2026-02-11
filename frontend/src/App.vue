<template>
  <v-app>
    <v-app-bar app color="primary" dark>
      <v-toolbar-title>ELET2415 Hydromonitor</v-toolbar-title>

      <v-spacer></v-spacer>

      <v-btn text to="/">Home</v-btn>
      <v-btn text to="/dashboard">Dashboard</v-btn>
      <v-btn text to="/control">Control</v-btn>
    </v-app-bar>

    <v-main>
      <router-view />
    </v-main>
  </v-app>
</template>

<script>
export default {
  name: 'App'
}
</script>

<style>
html, body, #app {
  height: 100%;
}
</style>
