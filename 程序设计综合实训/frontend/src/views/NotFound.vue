<!-- src/views/NotFound.vue -->

<template>
    <v-container>
      <v-row justify="center" align="center" style="height: 80vh;">
        <v-col class="text-center">
          <h1>404</h1>
          <p>页面未找到</p>
          <v-btn color="primary" to="/">返回首页</v-btn>
        </v-col>
      </v-row>
    </v-container>
  </template>
  
  <script>
  export default {
    name: 'NotFound',
  }
  </script>
  
  <style scoped>
  /* 可根据需要添加样式 */
  </style>
  