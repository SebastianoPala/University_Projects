package com.unipi.PlayerHive.DTO.users.login;

import jakarta.validation.constraints.Email;
import jakarta.validation.constraints.NotBlank;
import lombok.Data;

@Data
public class UserLoginDTO {
    @Email
    @NotBlank
    private String email;

    @NotBlank
    private String password;
    
}
