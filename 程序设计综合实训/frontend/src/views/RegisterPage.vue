<!-- src/views/RegisterPage.vue -->

<template>
  <UserRegister />
</template>

<script>
import UserRegister from '@/components/Auth/UserRegister.vue'

export default {
  name: 'RegisterPage',
  components: {
    UserRegister
  }
}
</script>

<style scoped>
/* 可根据需要添加样式 */
</style>
