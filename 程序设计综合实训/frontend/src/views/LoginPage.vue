<!-- src/views/LoginPage.vue -->

<template>
  <UserLogin />
</template>

<script>
import UserLogin from '@/components/Auth/UserLogin.vue'

export default {
  name: 'LoginPage',
  components: {
    UserLogin
  }
}
</script>

<style scoped>
/* 可根据需要添加样式 */
</style>
