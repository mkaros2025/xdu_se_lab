<!-- src/App.vue -->
<template>
  <v-app>
    <AppNavbar />
    <v-main>
      <v-container fluid>
        <!-- 页面内容 -->
        <transition name="fade">
          <router-view />
        </transition>
      </v-container>
    </v-main>
    <AppFooter />
  </v-app>
</template>

<script>
import AppNavbar from './components/Shared/AppNavbar.vue'

export default {
  name: 'App',
  components: {
    AppNavbar,
  },
}
</script>

<style>
/* 全局样式（如果需要） */
</style>
