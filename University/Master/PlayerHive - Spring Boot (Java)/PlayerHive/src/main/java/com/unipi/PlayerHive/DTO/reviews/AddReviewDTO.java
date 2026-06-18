package com.unipi.PlayerHive.DTO.reviews;

import jakarta.validation.constraints.*;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@AllArgsConstructor
@NoArgsConstructor
@Getter
@Setter

public class AddReviewDTO {

    @Size(max = 255)
    private String reviewText;

    @NotNull
    @Min(1)
    @Max(10)
    private Float score;
}
