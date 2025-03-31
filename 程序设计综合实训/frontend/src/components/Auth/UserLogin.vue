<!-- src/components/Auth/UserLogin.vue -->

<template>
  <v-container>
    <v-card class="mx-auto" max-width="500">
      <v-card-title>
        <span class="headline">登录</span>
      </v-card-title>
      <v-card-text>
        <v-form ref="form" v-model="valid" @submit.prevent="submit">
          <v-text-field
            v-model="username"
            label="用户名"
            :rules="[v => !!v || '用户名为必填项']"
            required
          ></v-text-field>

          <v-text-field
            v-model="password"
            label="密码"
            type="password"
            :rules="[v => !!v || '密码为必填项']"
            required
          ></v-text-field>

          <v-btn :disabled="!valid || loading" color="primary" type="submit" class="rounded-button">
            登录
          </v-btn>
        </v-form>
      </v-card-text>
      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn text @click="$router.push('/register')">还没有账户？注册</v-btn>
      </v-card-actions>
    </v-card>

    <!-- 显示消息 -->
    <v-snackbar v-model="snackbar" :color="snackbarColor" timeout="1000">
      {{ snackbarMessage }}
      <template v-slot:actions>
        <v-btn
          color="white"
          variant="text"
          @click="snackbar = false"
        >
          Close
        </v-btn>
      </template>
    </v-snackbar>
  </v-container>
</template>

<script>
import { mapActions } from 'vuex'

export default {
  name: 'UserLogin',
  data() {
    return {
      valid: false,
      username: '',
      password: '',
      loading: false,
      snackbar: false,
      snackbarMessage: '',
      snackbarColor: '',
    }
  },
  methods: {
    ...mapActions('auth', ['login']),
    async submit() {
      if (this.$refs.form.validate()) {
        this.loading = true
        try {
          const credentials = {
            username: this.username,
            password: this.password,
          }
          await this.login(credentials)
          this.snackbarMessage = '登录成功！'
          this.snackbarColor = 'success'
          this.snackbar = true
          console.log('登录成功，跳转首页')
          setTimeout(() => {
            this.$router.push('/')
          }, 50) // 0.5 秒后跳转
        } catch (error) {
          this.snackbarMessage = error.response?.data?.message || '登录失败，请检查用户名和密码。'
          this.snackbarColor = 'error'
          this.snackbar = true
          console.error('登录失败：', error)
        } finally {
          this.loading = false
        }
      }
    },
  },
}
</script>

<style scoped>
/* 可根据需要添加样式 */
.rounded-button {
  width: 100%;
  /* 按钮全宽 */
  margin-top: 6px;
  /* 增加按钮与上方输入框的间距 */
}
</style>
